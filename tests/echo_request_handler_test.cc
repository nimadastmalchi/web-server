#include "echo_request_handler.h"

#include "gtest/gtest.h"
#include "http_request.h"

class EchoRequestHandlerTest : public ::testing::Test {
    protected:
        void SetUp() override {}

        EchoRequestHandler handler_;
};

TEST_F(EchoRequestHandlerTest, ValidRequest) {
    std::string response;
    http_request req;
    std::string req_str = "GET /test/ HTTP/1.1\r\nName: Value";
    http_request::parseRequest(req, req_str);

    handler_.handleRequest(req, response);
    EXPECT_EQ(response,
              "HTTP/1.1 200 0K\r\nContent-Type: text/plain\r\n\r\nGET /test/ "
              "HTTP/1.1\r\nName: Value\r\n\r\n");
}
