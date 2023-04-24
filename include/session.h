#ifndef SESSION_H
#define SESSION_H

#include <cstdlib>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "response_builder.h"

using boost::asio::ip::tcp;

class session {
    public:
        session(boost::asio::io_service& io_service,
            ResponseBuilder& response_builder);
        tcp::socket& socket();
        void start();

    private:
        int handle_read(const boost::system::error_code& error,
            size_t bytes_transferred);
        int close_socket(const boost::system::error_code& error);

        tcp::socket socket_;
        ResponseBuilder& response_builder_;

        int bytes_read_;
        // Max length constant chosen to support 64K HTTP request size
        // Per: https://piazza.com/class/lfx37nmbiup24h/post/118
        enum { max_length = 64000 };
        char data_[max_length];
};

#endif