#include "session.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "response_builder.h"

using boost::asio::ip::tcp;

class SessionTest : public ::testing::Test {
    protected:
        void SetUp() override { io_service_.run(); }

        ResponseBuilder response_builder_;
        boost::asio::io_service io_service_;
        session session_ = session(tcp::socket(io_service_), response_builder_);
        const std::string delimiter_ = "\r\n\r\n";
        const std::string response_code_ = "HTTP/1.1 200 0K\r\n";
        const std::string content_type_ =
            "Content-Type: text/plain" + delimiter_;

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
    char data[] = "hello\n";
    write_(data);

    boost::system::error_code error;
    int ec = read_(error, len_());

    EXPECT_EQ(ec, 1);
}

TEST_F(SessionTest, CompleteWrite) {
    char data[] =
        "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nGET SOME "
        "DATA\r\n\r\n";
    write_(data);

    boost::system::error_code error;
    int ec = read_(error, len_());

    EXPECT_EQ(ec, 0);
}

TEST_F(SessionTest, SessionStart) {
    session_.start();
    char data[] = "start";
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