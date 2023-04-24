#include "response_builder.h"
#include "server.h"

#include <cstdlib>
#include <iostream>
#include <functional>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

server::server(boost::asio::io_service& io_service,
        tcp::acceptor& acceptor,
        ResponseBuilder& response_builder,
        std::function<session*(boost::asio::io_service&, ResponseBuilder&)> make_session)
    : io_service_(io_service),
      acceptor_(acceptor),
      response_builder_(response_builder),
      make_session_(std::move(make_session))
{
    start_accept();
}

void server::start_accept() {
    session* new_session = make_session_(io_service_, response_builder_);
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
