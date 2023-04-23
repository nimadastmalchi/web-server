#ifndef SERVER_H
#define SERVER_H

#include "response_builder.h"
#include "session.h"

#include <cstdlib>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

class server {
    public:
        server(boost::asio::io_service& io_service, 
            tcp::acceptor& acceptor,
            ResponseBuilder& response_builder);
    private:
        void start_accept();
        void handle_accept(session* new_session,
            const boost::system::error_code& error);

        boost::asio::io_service& io_service_;
        tcp::acceptor& acceptor_;
        ResponseBuilder& response_builder_;
};

#endif