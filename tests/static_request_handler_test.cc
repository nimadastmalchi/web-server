#include "static_request_handler.h"

#include <boost/beast/http.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <sstream>

#include "gtest/gtest.h"

using namespace boost::beast;

class StaticRequestHandlerTest : public ::testing::Test {
    protected:
        void SetUp() override {}

        StaticRequestHandler handler_ = StaticRequestHandler("static", "");
};

TEST_F(StaticRequestHandlerTest, FileNotFound) {
    http::request<http::string_body> req{http::verb::get,
                                         "/static/img/not_found", 11};
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    std::stringstream res_stream;
    res_stream << res;
    std::string response = res_stream.str();

    EXPECT_EQ(
        response,
        "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: "
        "87\r\n\r\n<html><head><title>404 Not Found</title></head><body><p>404 "
        "Not Found</p></body></html>");
}

TEST_F(StaticRequestHandlerTest, ValidRequest) {
    http::request<http::string_body> req{http::verb::get,
                                         "static/img/diary.png", 11};
    http::response<http::string_body> res;
    http::response<http::string_body> expected_res;

    std::string file_path = "./img/diary.png";
    std::ifstream f(file_path.c_str(), std::ios::in | std::ios::binary);

    std::string expected_file_body;
    char c;
    while (f.get(c)) expected_file_body += c;
    f.close();

    expected_res.result(http::status::ok);
    expected_res.version(11);
    expected_res.set(http::field::content_type, "image/png");
    expected_res.set(http::field::content_length,
                     std::to_string(expected_file_body.length()));
    expected_res.body() = std::move(expected_file_body);
    expected_res.prepare_payload();

    handler_.handle_request(req, res);
    std::stringstream res_stream;
    res_stream << res;
    std::string response = res_stream.str();
    std::stringstream expected_res_stream;
    expected_res_stream << expected_res;
    std::string expected_response = expected_res_stream.str();
    EXPECT_EQ(response, expected_response);
}
