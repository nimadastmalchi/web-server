#include "echo_request_handler.h"

#include <boost/beast/http.hpp>
#include <sstream>

#include "gtest/gtest.h"

using namespace boost::beast;

class EchoRequestHandlerTest : public ::testing::Test {
    protected:
        void SetUp() override {}

        EchoRequestHandler handler_;
};

TEST_F(EchoRequestHandlerTest, ValidRequest) {
    http::request<http::string_body> req{http::verb::get, "test", 11};
    req.set("Name", "Value");

    http::response<http::string_body> res;
    handler_.handle_request(req, res);

    std::stringstream res_stream;
    res_stream << res;
    std::string response = res_stream.str();
    EXPECT_EQ(response,
              "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
              "34\r\n\r\nGET test HTTP/1.1\r\nName: Value\r\n\r\n");
}
