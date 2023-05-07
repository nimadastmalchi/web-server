#include "session.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "http_request.h"
#include "logger.h"
#include "request_handler.h"
#include "response_builder.h"

using boost::asio::ip::tcp;

session::session(
    tcp::socket socket, ResponseBuilder& response_builder,
    std::map<std::string, std::shared_ptr<RequestHandler>> handlers)
    : socket_(std::move(socket)),
      response_builder_(response_builder),
      bytes_read_(0),
      handlers_(std::move(handlers)) {}

tcp::socket& session::socket() { return socket_; }

void session::start() {
    Logger::log_trace(socket(), "Session is ready to read from socket");
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

        const std::string delimiter = "\r\n\r\n";
        const std::string response_code = "HTTP/1.1 200 0K\r\n";
        const std::string content_type = "Content-Type: text/plain" + delimiter;

        // If we have received the entire header, write the response:
        if (response_builder_.formatResponse(data_, delimiter, response_code,
                                             content_type)) {
            Logger::log_trace(socket(), "Received entire header");
            // WIP -- Parse the request:
            http_request req;
            if (http_request::parseRequest(req, std::string(data_))) {
                Logger::log_trace(socket(), "Received request: " + req.method +
                                                " " + req.uri);

                std::shared_ptr<RequestHandler> handler =
                    getRequestHandler(req);
                if (handler != nullptr) {
                    Logger::log_trace(socket(), "Valid request");

                    handler->handleRequest(req, response_);
                    boost::asio::async_write(
                        socket_,
                        boost::asio::buffer(response_, response_.size()),
                        boost::bind(&session::close_socket, this,
                                    boost::asio::placeholders::error));
                } else {
                    Logger::log_trace(socket(),
                                      "Unknown URI, responding with 400");

                    std::string http_version =
                        "HTTP/" + std::to_string(req.http_version_major) + "." +
                        std::to_string(req.http_version_minor);

                    std::string res = http_version + " 400 BAD REQUEST\r\n\r\n";

                    boost::asio::async_write(
                        socket_, boost::asio::buffer(res, res.size()),
                        boost::bind(&session::close_socket, this,
                                    boost::asio::placeholders::error));
                }

            } else {
                Logger::log_trace(socket(),
                                  "Bad HTTP request, responding with 400");

                std::string http_version =
                    "HTTP/" + std::to_string(req.http_version_major) + "." +
                    std::to_string(req.http_version_minor);

                std::string res = http_version + " 400 BAD REQUEST\r\n\r\n";

                boost::asio::async_write(
                    socket_, boost::asio::buffer(res, res.size()),
                    boost::bind(&session::close_socket, this,
                                boost::asio::placeholders::error));
            }
            return 0;
            // If we have not reached the end of the header, continue reading:
        } else {
            Logger::log_trace(socket(),
                              "End of header not received, continue reading");
            socket_.async_read_some(
                boost::asio::buffer(data_ + bytes_read_,
                                    max_length - bytes_read_),
                boost::bind(&session::handle_read, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));

            return 1;
        }
    } else {
        Logger::log_error(socket(),
                          "Error in session when reading from socket");
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
        response_builder_.resetResponse();
        Logger::log_trace(socket(), "Closing session");
        socket_.close();

        return 0;
    } else {
        Logger::log_error(socket(), "Failed to send response, closing session");
        delete this;

        return 1;
    }
}

std::shared_ptr<RequestHandler> session::getRequestHandler(
    const http_request& req) {
    std::string uri = req.uri;

    std::shared_ptr<RequestHandler> handler = nullptr;
    std::string prefix;

    for (auto it = handlers_.begin(); it != handlers_.end(); ++it) {
        if (uri.substr(0, it->first.length()) == it->first) {
            if (it->first.length() >= prefix.length()) {
                prefix = it->first;
                handler = it->second;
            }
        }
    }

    return handler;
}
