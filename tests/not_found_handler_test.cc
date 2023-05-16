#include "not_found_handler.h"

#include <boost/beast/http.hpp>
#include <sstream>

#include "gtest/gtest.h"

using namespace boost::beast;

class NotFoundHandlerTest : public ::testing::Test {
    protected:
        void SetUp() override {}

        NotFoundHandler handler_;
};

TEST_F(NotFoundHandlerTest, UnsupportedLocation) {
    http::request<http::string_body> req{http::verb::get, "test", 11};
    http::response<http::string_body> res;

    handler_.handle_request(req, res);

    std::stringstream req_stream;
    req_stream << res;
    std::string response = req_stream.str();

    EXPECT_EQ(
        response,
        "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: "
        "87\r\n\r\n<html><head><title>404 Not Found</title></head><body><p>404 "
        "Not Found</p></body></html>");
}
