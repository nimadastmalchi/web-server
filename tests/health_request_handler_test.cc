#include "health_request_handler.h"

#include <boost/beast/http.hpp>
#include <sstream>

#include "gtest/gtest.h"

using namespace boost::beast;

class HealthRequestHandlerTest : public ::testing::Test {
    protected:
        void SetUp() override {}

        HealthRequestHandler handler_;
};

TEST_F(HealthRequestHandlerTest, ValidRequest) {
    http::request<http::string_body> req{http::verb::get, "health", 11};

    http::response<http::string_body> res;
    handler_.handle_request(req, res);

    std::stringstream res_stream;
    res_stream << res;
    std::string response = res_stream.str();
    EXPECT_EQ(response,
              "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
              "2\r\n\r\nOK");
}
