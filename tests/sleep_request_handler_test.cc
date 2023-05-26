#include "sleep_request_handler.h"

#include <boost/beast/http.hpp>
#include <chrono>
#include <sstream>

#include "gtest/gtest.h"

using namespace boost::beast;

class SleepRequestHandlerTest : public ::testing::Test {
    protected:
        void SetUp() override {}

        SleepRequestHandler handler_ = SleepRequestHandler("sleep", 1);
};

TEST_F(SleepRequestHandlerTest, FileNotFound) {
    http::request<http::string_body> req{http::verb::get, "/sleep", 11};
    http::response<http::string_body> res;

    auto start = std::chrono::steady_clock::now();  // milliseconds
    handler_.handle_request(req, res);
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                       std::chrono::steady_clock::now() - start)
                       .count();
    EXPECT_TRUE(elapsed >= 1);
}