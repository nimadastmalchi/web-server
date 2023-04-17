#ifndef SESSION_H
#define SESSION_H

#include <cstdlib>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

class session {
    public:
        session(boost::asio::io_service& io_service);
        tcp::socket& socket();
        void start();

    private:
        void handle_read(const boost::system::error_code& error,
            size_t bytes_transferred);
        void handle_write(const boost::system::error_code& error);

        void handle_request_read(const boost::system::error_code& error,
            size_t bytes_transferred);
        void handle_request_write(const boost::system::error_code& error);

        tcp::socket socket_;

        // Max length constant chosen based to support 64K HTTP request size
        // Per: https://piazza.com/class/lfx37nmbiup24h/post/118
        enum { max_length = 64000 };
        char data_[max_length];
        std::string response_;
};

#endif