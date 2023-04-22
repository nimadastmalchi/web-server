#include "server.h"

#include <cstdlib>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

server::server(boost::asio::io_service& io_service, tcp::acceptor& acceptor)
    : io_service_(io_service), acceptor_(acceptor)
{
    start_accept();
}

void server::start_accept() {
    session* new_session = new session(io_service_);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
        boost::asio::placeholders::error));
}

void server::handle_accept(session* new_session,
        const boost::system::error_code& error) {
    if (!error) {
        new_session->start();
    }
    else {
        delete new_session;
    }

    start_accept();
}
