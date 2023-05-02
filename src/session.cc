#include "session.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

#include "http_request.h"
#include "logger.h"
#include "response_builder.h"

using boost::asio::ip::tcp;

session::session(tcp::socket socket, ResponseBuilder& response_builder)
    : socket_(std::move(socket)),
      response_builder_(response_builder),
      bytes_read_(0) {}

tcp::socket& session::socket() { return socket_; }

void session::start() {
    Logger::log_trace("Session is ready to read from socket");
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
            Logger::log_trace("Received entire header");
            // WIP -- Parse the request:
            http_request req;
            if (http_request::parseRequest(req, std::string(data_))) {
                std::cout << "HTTP Header" << std::endl;
                std::cout << "\tMethod: " << req.method << std::endl;
                std::cout << "\tURI: " << req.uri << std::endl;
                std::cout << "\tMajor version: " << req.http_version_major
                          << std::endl;
                std::cout << "\tMinor version: " << req.http_version_minor
                          << std::endl;
                for (auto header : req.headers) {
                    std::cout << "\t" << header.name << ": " << header.value
                              << std::endl;
                }
            }
            std::string response = response_builder_.getResponse();
            Logger::log_trace("Sending response");
            boost::asio::async_write(
                socket_, boost::asio::buffer(response, response.size()),
                boost::bind(&session::close_socket, this,
                            boost::asio::placeholders::error));

            return 0;
            // If we have not reached the end of the header, continue reading:
        } else {
            Logger::log_trace("End of header not received, continue reading");
            socket_.async_read_some(
                boost::asio::buffer(data_ + bytes_read_,
                                    max_length - bytes_read_),
                boost::bind(&session::handle_read, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));

            return 1;
        }
    } else {
        Logger::log_error("Error in session when reading from socket");
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
        Logger::log_trace("Closing session");
        socket_.close();

        return 0;
    } else {
        Logger::log_error("Failed to send response, closing session");
        delete this;

        return 1;
    }
}
