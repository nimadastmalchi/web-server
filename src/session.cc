#include "session.h"

#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <regex>
#include <string>

#include "logger.h"
#include "request_handler.h"
#include "request_handler_factory.h"

using boost::asio::ip::tcp;
using namespace boost::beast;

std::string stripSlashes(std::string target) {
    while (!target.empty() && target[0] == '/') {
        target = target.substr(1);
    }
    while (!target.empty() && target.back() == '/') {
        target.pop_back();
    }
    return target;
}

session::session(tcp::socket socket,
                 std::map<std::string, std::shared_ptr<RequestHandlerFactory>>
                     handlerFactories)
    : socket_(std::move(socket)),
      bytes_read_(0),
      handlerFactories_(std::move(handlerFactories)) {}

tcp::socket& session::socket() { return socket_; }

void session::start() {
    Logger::log_trace(socket_, "Session is ready to read from socket");
    socket_.async_read_some(
        boost::asio::buffer(data_ + bytes_read_, max_length - bytes_read_),
        boost::bind(&session::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

// Exit codes:
// 0: End of header reached.
// 1: End of header not yet reached.
// 2: Error.
int session::handle_read(const boost::system::error_code& error,
                         size_t bytes_transferred) {
    if (!error) {
        bytes_read_ += bytes_transferred;

        http::request<http::string_body> request;
        http::request_parser<http::string_body> request_parser;
        error_code request_parser_code;
        request_parser.put(boost::asio::buffer(data_, bytes_read_),
                           request_parser_code);

        if (request_parser_code == boost::system::errc::success) {
            request = request_parser.get();
            if (request.has_content_length()) {
                int content_length =
                    stoi(std::string(request.find("Content-Length")->value()));
                if (content_length > 0) {
                    data_[bytes_transferred] = 0;
                    std::string body =
                        std::string(&data_[bytes_transferred - content_length]);
                    request.body() = body;
                }
            }
            std::string initial_target = request.target().to_string();
            // Remove leading and trailing slashes:
            request.target(stripSlashes(initial_target));
            std::stringstream log_stream;
            log_stream << "Received request: " << request.method() << " "
                       << request.target();
            Logger::log_trace(socket_, log_stream.str());
            std::shared_ptr<RequestHandlerFactory> handlerFactory =
                get_handler_factory(request);
            if (handlerFactory != nullptr) {
                try {
                    std::string address =
                        socket_.remote_endpoint().address().to_string();
                    handlerFactory->setAddress(address);
                    log_stream << "Client IP address: " << address;
                } catch (...) {
                    // No remote_endpoint, so no ip address
                    log_stream << "Cannot resolve remote endpoint";
                }
                // Create a new handler for each request:
                std::shared_ptr<RequestHandler> handler =
                    handlerFactory->createHandler();
                http::response<http::string_body> response;
                handler->handle_request(request, response);

                // Log for metrics:
                log_metrics_info(response.result_int(), initial_target,
                                 handlerFactory->getHandlerName());

                std::stringstream res_stream;
                res_stream << response;
                responseStr_ = res_stream.str();

                boost::asio::async_write(
                    socket_,
                    boost::asio::buffer(responseStr_, responseStr_.size()),
                    boost::bind(&session::close_socket, this,
                                boost::asio::placeholders::error));
                return 0;
            } else {
                // If a handler could not be found, the URI is unsupported:
                Logger::log_trace(socket_, "Unknown URI, responding with 400");

                http::response<http::empty_body> errorResponse;
                errorResponse.version(request.version());
                errorResponse.result(http::status::bad_request);
                errorResponse.prepare_payload();

                // Log for metrics:
                log_metrics_info(400, initial_target, "Unknown");

                std::stringstream res_stream;
                res_stream << errorResponse;
                responseStr_ = res_stream.str();

                boost::asio::async_write(
                    socket_,
                    boost::asio::buffer(responseStr_, responseStr_.size()),
                    boost::bind(&session::close_socket, this,
                                boost::asio::placeholders::error));
                return 2;
            }
        } else if (request_parser_code == http::error::need_more) {
            // If the end of the request has not yet been reached, continue
            // reading:
            Logger::log_trace(socket_,
                              "End of request not received, continue reading");
            socket_.async_read_some(
                boost::asio::buffer(data_ + bytes_read_,
                                    max_length - bytes_read_),
                boost::bind(&session::handle_read, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));

            return 1;
        } else {
            // If the request failed to parse, send an error response:
            Logger::log_trace(socket_, "Bad HTTP request, responding with 400");

            http::response<http::empty_body> errorResponse;
            errorResponse.version(request.version());
            errorResponse.result(http::status::bad_request);
            errorResponse.prepare_payload();

            // Log for metrics:
            log_metrics_info(400, "Unknown", "Unknown");

            std::stringstream res_stream;
            res_stream << errorResponse;
            responseStr_ = res_stream.str();

            boost::asio::async_write(
                socket_, boost::asio::buffer(responseStr_, responseStr_.size()),
                boost::bind(&session::close_socket, this,
                            boost::asio::placeholders::error));
            return 2;
        }
    } else {
        Logger::log_error(socket_, "Error in session when reading from socket");
        delete this;

        return 2;
    }
}

// Exit codes:
// 0: Socket closed.
// 1: Error.
int session::close_socket(const boost::system::error_code& error) {
    if (!error) {
        // Upon writing, reset all instance variables:
        memset(data_, 0, bytes_read_);
        bytes_read_ = 0;
        Logger::log_trace(socket_, "Closing session");
        socket_.close();

        return 0;
    } else {
        Logger::log_error(socket_, "Failed to send response, closing session");
        delete this;

        return 1;
    }
}

std::shared_ptr<RequestHandlerFactory> session::get_handler_factory(
    const http::request<http::string_body>& req) {
    std::string uri = req.target().to_string();
    std::shared_ptr<RequestHandlerFactory> handlerFactory = nullptr;
    std::string prefix;

    for (auto it = handlerFactories_.begin(); it != handlerFactories_.end();
         ++it) {
        if (uri.substr(0, it->first.length()) == it->first) {
            if (it->first.length() >= prefix.length()) {
                prefix = it->first;
                handlerFactory = it->second;
            }
        }
    }

    return handlerFactory;
}

void session::log_metrics_info(int response_code,
                               const std::string& request_path,
                               const std::string& handler_name) {
    std::string address = socket_.is_open()
                              ? socket_.remote_endpoint().address().to_string()
                              : "Unknown";
    std::string metrics_info =
        "[ResponseMetrics] (Code: " + std::to_string(response_code) +
        ") (Path: " + request_path + ") (IP: " + address +
        ") (Handler: " + handler_name + ")";
    Logger::log_trace(metrics_info);
}
