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

bool validHeaderName(const std::string& headerName) {
    int n = headerName.size();
    if (n == 0) {
        return false;
    }
    if (!isalpha(headerName[0]) || !isalpha(headerName[n - 1])) {
        return false;
    }
    bool lastIsHyphen = false;
    for (auto c : headerName) {
        if (lastIsHyphen && c == '-') {
            return false;
        }
        lastIsHyphen = (c == '-');
        if (!isalpha(c) && c != '-') {
            return false;
        }
    }
    return true;
}

bool parseRawRequest(const std::string& req_str,
                     http::request<http::string_body>& request) {
    std::istringstream req_stream(req_str);

    std::string line;
    if (!std::getline(req_stream, line)) {
        return false;
    }

    // Parse method, URI, and http method from first line:
    std::istringstream first_line(line);
    std::string http_version_str, method, uri;
    if (!(first_line >> method >> uri >> http_version_str)) {
        return false;
    }

    while (!uri.empty() && uri[0] == '/') {
        uri = uri.substr(1);
    }
    while (!uri.empty() && uri.back() == '/') {
        uri.pop_back();
    }

    int major, minor;
    if (std::sscanf(http_version_str.c_str(), "HTTP/%d.%d", &major, &minor) !=
        2) {
        return false;
    }

    // Ensure the method in the header is valid:
    if (method == "GET") {
        request.method(http::verb::get);
    }
    else if(method =="POST"){
        request.method(http::verb::post);
    }
    else if(method =="PUT"){
        request.method(http::verb::put);
    }
    else if(method =="DELETE"){
        request.method(http::verb::delete_);
    }
    else{
        return false;
    }

    
    request.target(uri);
    int v = std::stoi(std::to_string(major) + std::to_string(minor));
    request.version(v);

    // Parse the headers (read the rest of the request string):
    while (std::getline(req_stream, line)) {
        std::string name, value;
        std::istringstream header_line(line);
        if (std::getline(header_line, name, ':') &&
            std::getline(header_line, value)) {
            if (!validHeaderName(name)) {
                return false;
            }
            // Trim value:
            value = std::regex_replace(value, std::regex("^ +| +$"), "$1");
            request.insert(name, value);

        } else {
            return false;
        }
    }

    return true;
}

session::session(tcp::socket socket,
                 std::map<std::string, std::shared_ptr<RequestHandlerFactory>>
                     handlerFactories)
    : socket_(std::move(socket)),
      bytes_read_(0),
      handlerFactories_(std::move(handlerFactories)) {}

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
        // Check whether the delimiter exists:
        char* header_end = strstr(data_, delimiter.c_str());

        // If we have received the entire header, write the response:
        if (header_end) {
            // Delimit the end of the header with NULL:
            data_[header_end - data_] = 0;

            Logger::log_trace(socket(), "Received entire header");

            http::request<http::string_body> request;
            if (parseRawRequest(std::string(data_), request)) {
                std::stringstream log_stream;
                log_stream << "Received request: " << request.method() << " "
                           << request.target();
                Logger::log_trace(socket(), log_stream.str());
                std::shared_ptr<RequestHandlerFactory> handlerFactory =
                    getRequestHandlerFactory(request);
                if (handlerFactory != nullptr) {
                    // Create a new handler for each request:
                    std::shared_ptr<RequestHandler> handler =
                        handlerFactory->createHandler();
                    http::response<http::string_body> response;
                    handler->handle_request(request, response);
                    std::stringstream res_stream;
                    res_stream << response;
                    responseStr_ = res_stream.str();
                    boost::asio::async_write(
                        socket_,
                        boost::asio::buffer(responseStr_, responseStr_.size()),
                        boost::bind(&session::close_socket, this,
                                    boost::asio::placeholders::error));
                } else {
                    Logger::log_trace(socket(),
                                      "Unknown URI, responding with 400");

                    http::response<http::empty_body> errorResponse;
                    errorResponse.version(request.version());
                    errorResponse.result(http::status::bad_request);
                    errorResponse.prepare_payload();

                    std::stringstream res_stream;
                    res_stream << errorResponse;
                    responseStr_ = res_stream.str();

                    boost::asio::async_write(
                        socket_,
                        boost::asio::buffer(responseStr_, responseStr_.size()),
                        boost::bind(&session::close_socket, this,
                                    boost::asio::placeholders::error));
                }
            } else {
                Logger::log_trace(socket(),
                                  "Bad HTTP request, responding with 400");

                http::response<http::empty_body> errorResponse;
                errorResponse.version(request.version());
                errorResponse.result(http::status::bad_request);
                errorResponse.prepare_payload();

                std::stringstream res_stream;
                res_stream << errorResponse;
                responseStr_ = res_stream.str();

                boost::asio::async_write(
                    socket_,
                    boost::asio::buffer(responseStr_, responseStr_.size()),
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
        Logger::log_trace(socket(), "Closing session");
        socket_.close();

        return 0;
    } else {
        Logger::log_error(socket(), "Failed to send response, closing session");
        delete this;

        return 1;
    }
}

std::shared_ptr<RequestHandlerFactory> session::getRequestHandlerFactory(
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