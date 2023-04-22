#include <cstdlib>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "session.h"

using boost::asio::ip::tcp;

session::session(boost::asio::io_service& io_service)
    : socket_(io_service), bytes_read_(0)
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

void session::handle_read(const boost::system::error_code& error,
      size_t bytes_transferred) {
    if (!error) {
        bytes_read_ += bytes_transferred;
        
        const std::string header_delimiter = "\r\n\r\n";
        const std::string response_code = "HTTP/1.1 200 0K\r\n";
        const std::string content_type = "Content-Type: text/plain" + header_delimiter;

        // Check whether 2x CRLF exists:
        char * header_end = strstr(data_, header_delimiter.c_str());

        // If it does exist, we have received the entire header:
        if (header_end) {
            int header_size = header_end - data_;

            // Delimit the end of the header with NULL:
            data_[header_size] = 0;
            std::string content = data_;

            // Construct and send the response:
            response_ = response_code + content_type + content + "\r\n";
            boost::asio::async_write(socket_, 
                boost::asio::buffer(response_, response_.size()),
                boost::bind(&session::close_socket, this,
                boost::asio::placeholders::error));
        // If we have not reached the end of the header, continue reading:
        } else {
            socket_.async_read_some(boost::asio::buffer(data_ + bytes_read_, max_length - bytes_read_),
                boost::bind(&session::handle_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        }
    }
    else {
        delete this;
    }
}

void session::close_socket(const boost::system::error_code& error) {
    if (!error) {
        // Upon writing, reset all instance variables:
        memset(data_, 0, bytes_read_);
        bytes_read_ = 0;

        socket_.close();
    }
    else {
        delete this;
    }
}
