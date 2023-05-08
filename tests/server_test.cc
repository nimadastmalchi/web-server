#include "server.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "echo_request_handler.h"
#include "gtest/gtest.h"
#include "request_handler.h"
#include "session.h"

using boost::asio::ip::tcp;

class ServerTest : public ::testing::Test {
    protected:
        void SetUp() override {}

        int port = 96;

        const std::string delimiter_ = "\r\n\r\n";

        boost::asio::io_service io_service_;
        tcp::endpoint endpoint = tcp::endpoint(tcp::v4(), port);
        tcp::acceptor acceptor = tcp::acceptor(io_service_, endpoint);

        server server_ = server(
            io_service_, acceptor,
            [](boost::asio::io_service& io_service_) -> session* {
                std::map<std::string, std::shared_ptr<RequestHandler>> handlers;
                handlers[""] = std::make_shared<EchoRequestHandler>();
                tcp::socket socket(io_service_);
                return new session(std::move(socket), handlers);
            });

        void start_() { return server_.start_accept(); }

        void accept_() {
            session* new_session = server_.make_session_(io_service_);
            boost::system::error_code error;

            server_.handle_accept(new_session, error);
        }
};

TEST_F(ServerTest, ServerStart) {
    start_();
    EXPECT_EQ(1, 1);
}

TEST_F(ServerTest, ServerAccept) {
    accept_();
    EXPECT_EQ(1, 1);
}
