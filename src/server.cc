#include "server.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>

#include "logger.h"

using boost::asio::ip::tcp;

server::server(boost::asio::io_service& io_service, tcp::acceptor& acceptor,
               std::function<session*(boost::asio::io_service&)> make_session)
    : io_service_(io_service),
      acceptor_(acceptor),
      make_session_(std::move(make_session)) {
    start_accept();
}

void server::start_accept() {
    session* new_session = make_session_(io_service_);
    Logger::log_trace("Opened new session");
    acceptor_.async_accept(
        new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
                    boost::asio::placeholders::error));
}

void server::handle_accept(session* new_session,
                           const boost::system::error_code& error) {
    if (!error) {
        new_session->start();
    } else {
        Logger::log_error("Error opening new session");
        delete new_session;
    }

    start_accept();
}
