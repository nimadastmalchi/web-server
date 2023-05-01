#include "http_request.h"
#include "gtest/gtest.h"

class HttpRequestTest : public ::testing::Test {
 protected:
  void SetUp() override { }

  http_request req;
};

TEST_F(HttpRequestTest, ValidRequest) {
  const std::string& req_str = "GET /test/ HTTP/1.1\r\nName: Value";

  bool success = http_request::parseRequest(req, req_str);
  EXPECT_TRUE(success);
  EXPECT_EQ(req.method, "GET");
  EXPECT_EQ(req.uri, "/test/");
  EXPECT_EQ(req.http_version_major, 1);
  EXPECT_EQ(req.http_version_minor, 1);
  EXPECT_EQ(req.headers[0].name, "Name");
  EXPECT_EQ(req.headers[0].value, "Value");
}

TEST_F(HttpRequestTest, EmptyRequest) {
  const std::string& req_str = "";

  bool success = http_request::parseRequest(req, req_str);
  EXPECT_FALSE(success);
}

TEST_F(HttpRequestTest, InvalidStartLineRequest) {
  const std::string& req_str = "GET HTTP/1.1\r\nName: Value";

  bool success = http_request::parseRequest(req, req_str);
  EXPECT_FALSE(success);
}

TEST_F(HttpRequestTest, InvalidMethodRequest) {
  const std::string& req_str = "POST /test/ HTTP/1.1\r\nName: Value";

  bool success = http_request::parseRequest(req, req_str);
  EXPECT_FALSE(success);
}

TEST_F(HttpRequestTest, InvalidHttpVersionRequest) {
  const std::string& req_str = "GET /test/ HTTP/2\r\nName: Value";

  bool success = http_request::parseRequest(req, req_str);
  EXPECT_FALSE(success);
}

TEST_F(HttpRequestTest, InvalidHeaderFormatRequest) {
  const std::string& req_str = "GET /test/ HTTP/2\r\nTest";

  bool success = http_request::parseRequest(req, req_str);
  EXPECT_FALSE(success);
}

TEST_F(HttpRequestTest, InvalidHeaderNameRequest) {
  const std::string& req_str = "GET /test/ HTTP/2\r\n123: 456";

  bool success = http_request::parseRequest(req, req_str);
  EXPECT_FALSE(success);
}
