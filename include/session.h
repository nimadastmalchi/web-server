#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>

#include "request_handler.h"
#include "response_builder.h"

using boost::asio::ip::tcp;

class session {
    public:
        session(
            tcp::socket socket, ResponseBuilder& response_builder,
            std::map<std::string, std::shared_ptr<RequestHandler>> handlers);
        tcp::socket& socket();
        void start();

        friend class SessionTest;

    private:
        int handle_read(const boost::system::error_code& error,
                        size_t bytes_transferred);
        int close_socket(const boost::system::error_code& error);
        std::shared_ptr<RequestHandler> getRequestHandler(
            const http_request& req);

        tcp::socket socket_;
        ResponseBuilder& response_builder_;

        std::string response_;

        int bytes_read_;
        // Max length constant chosen to support 64K HTTP request size
        // Per: https://piazza.com/class/lfx37nmbiup24h/post/118
        enum { max_length = 64000 };
        char data_[max_length];

        std::map<std::string, std::shared_ptr<RequestHandler>> handlers_;
};

#endif