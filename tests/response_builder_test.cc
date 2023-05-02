#include "response_builder.h"

#include "gtest/gtest.h"

class ResponseBuilderTest : public ::testing::Test {
    protected:
        void SetUp() override {}

        ResponseBuilder response_builder_;
        const std::string delimiter_ = "\r\n\r\n";
        const std::string response_code_ = "HTTP/1.1 200 0K\r\n";
        const std::string content_type_ =
            "Content-Type: text/plain" + delimiter_;
};

TEST_F(ResponseBuilderTest, SimpleRequestAndReset) {
    char data[] = "This is a sample request.\r\n\r\n";

    int return_value = response_builder_.formatResponse(
        data, delimiter_, response_code_, content_type_);

    EXPECT_EQ(return_value, 1);
    EXPECT_EQ(response_builder_.getResponse(),
              "HTTP/1.1 200 0K\r\nContent-Type: text/plain\r\n\r\nThis is a "
              "sample request.\r\n");

    response_builder_.resetResponse();

    EXPECT_EQ(response_builder_.getResponse(), "");
}

TEST_F(ResponseBuilderTest, NoDelimiterRequest) {
    char data[] = "This is a sample request.";

    int return_value = response_builder_.formatResponse(
        data, delimiter_, response_code_, content_type_);

    EXPECT_EQ(return_value, 0);
    EXPECT_EQ(response_builder_.getResponse(), "");
}
