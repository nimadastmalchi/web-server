#include "static_request_handler.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "gtest/gtest.h"
#include "http_request.h"

class StaticRequestHandlerTest : public ::testing::Test {
    protected:
        void SetUp() override {}
        std::string prefix = "static";
        std::string root = "";
        std::string req_str;

        char c;
        StaticRequestHandler handler = StaticRequestHandler(root, prefix);
};

TEST_F(StaticRequestHandlerTest, FileNotFound) {
    std::string response;
    http_request req;
    std::string req_str = "GET /static/img/not_found HTTP/1.1\r\nName: Value";
    http_request::parseRequest(req, req_str);

    handler.handleRequest(req, response);
    EXPECT_EQ(
        response,
        "HTTP/1.1 404 Not Found\r\nContent-Type: "
        "text/html\r\n\r\n<html><head><title>404 Not "
        "Found</title></head><body><p>404 Not Found</p></body></html>\r\n");
}

TEST_F(StaticRequestHandlerTest, ValidRequest) {
    std::string response;
    http_request req;
    req_str = "GET /static/img/diary.png HTTP/1.1\r\nName: Value";
    http_request::parseRequest(req, req_str);

    std::string file_path = "./img/diary.png";
    std::ifstream f(file_path.c_str(), std::ios::in | std::ios::binary);

    std::string expected_file_body;
    while (f.get(c)) expected_file_body += c;
    f.close();

    std::string expected = "HTTP/1.1 200 OK\r\nContent-Length: " +
                           std::to_string(expected_file_body.length()) +
                           "\r\nContent-Type: image/png\r\n\r\n" +
                           expected_file_body + "\r\n";
    handler.handleRequest(req, response);
    EXPECT_EQ(response, expected);
}
