#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <iostream>

#include "session.h"

using boost::asio::ip::tcp;

class server {
    public:
        server(boost::asio::io_service& io_service, tcp::acceptor& acceptor,
               std::function<session*(boost::asio::io_service&)> make_session);

        friend class ServerTest;

    private:
        void start_accept();
        void handle_accept(session* new_session,
                           const boost::system::error_code& error);

        boost::asio::io_service& io_service_;
        tcp::acceptor& acceptor_;
        std::function<session*(boost::asio::io_service&)> make_session_;
};

#endif