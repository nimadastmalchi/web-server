#include "response_builder.h"
#include "session.h"

#include <cstdlib>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

session::session(tcp::socket socket,
        ResponseBuilder& response_builder)
    : socket_(std::move(socket)),
      response_builder_(response_builder),
      bytes_read_(0)
{}

tcp::socket& session::socket() {
    return socket_;
}

void session::start() {
    socket_.async_read_some(boost::asio::buffer(data_ + bytes_read_, max_length - bytes_read_),
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
        if (response_builder_.formatResponse(data_, delimiter, response_code, content_type)) {
            std::string response = response_builder_.getResponse();
            boost::asio::async_write(socket_,
                boost::asio::buffer(response, response.size()),
                boost::bind(&session::close_socket, this,
                boost::asio::placeholders::error));

            return 0;
        // If we have not reached the end of the header, continue reading:
        } else {
            socket_.async_read_some(boost::asio::buffer(data_ + bytes_read_, max_length - bytes_read_),
                boost::bind(&session::handle_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
            
            return 1;
        }
    }
    else {
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

        socket_.close();

        return 0;
    }
    else {
        delete this;

        return 1;
    }
}
