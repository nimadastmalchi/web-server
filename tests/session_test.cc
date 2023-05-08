#include "session.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "echo_request_handler.h"
#include "gtest/gtest.h"
#include "request_handler.h"

using boost::asio::ip::tcp;

class SessionTest : public ::testing::Test {
    protected:
        void SetUp() override {
            io_service_.run();
            handlers[""] = std::make_shared<EchoRequestHandler>();
        }

        std::map<std::string, std::shared_ptr<RequestHandler>> handlers;
        boost::asio::io_service io_service_;
        session session_ = session(tcp::socket(io_service_), handlers);

        int close_(boost::system::error_code& error) {
            return session_.close_socket(error);
        }

        int read_(const boost::system::error_code& error,
                  size_t bytes_transferred) {
            return session_.handle_read(error, bytes_transferred);
        }

        void write_(char* data) { sprintf(session_.data_, "%s", data); }

        size_t len_() { return strlen(session_.data_); }
};

TEST_F(SessionTest, IncompleteWrite) {
    char data[] = "Hello\n";
    write_(data);

    boost::system::error_code error;
    int ec = read_(error, len_());

    EXPECT_EQ(ec, 1);
}

TEST_F(SessionTest, CompleteWrite) {
    char data[] =
        "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nData\r\n\r\n";
    write_(data);

    boost::system::error_code error;
    int ec = read_(error, len_());

    EXPECT_EQ(ec, 0);
}

TEST_F(SessionTest, SessionStart) {
    session_.start();
    char data[] = "Start";
    write_(data);
    session_.socket();
    boost::system::error_code error;
    int ec = read_(error, len_());

    EXPECT_EQ(ec, 1);
}

TEST_F(SessionTest, CloseSocketSuccess) {
    boost::system::error_code error;
    int ec = close_(error);

    EXPECT_EQ(ec, 0);
}
