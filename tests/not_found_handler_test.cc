#include "not_found_handler.h"

#include "gtest/gtest.h"
#include "http_request.h"

class NotFoundHandlerTest : public ::testing::Test {
    protected:
        void SetUp() override {}

        NotFoundHandler handler_;
};

TEST_F(NotFoundHandlerTest, UnsupportedLocation) {
    std::string response;
    http_request req;
    std::string req_str = "GET /test/ HTTP/1.1\r\nName: Value";
    http_request::parseRequest(req, req_str);

    handler_.handleRequest(req, response);
    EXPECT_EQ(response,
              "HTTP/1.1 404 Not Found\r\nContent-Type: "
              "text/html\r\n\r\n<html><head><title>404 Not "
              "Found</title></head><body><p>404 Not "
              "Found</p></body></html>\r\n");
}
