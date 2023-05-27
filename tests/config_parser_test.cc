#include "config_parser.h"

#include "crud_request_handler_factory.h"
#include "echo_request_handler_factory.h"
#include "gtest/gtest.h"
#include "not_found_handler_factory.h"
#include "request_handler_factory.h"
#include "sleep_request_handler_factory.h"
#include "static_request_handler_factory.h"

class NginxConfigParserTest : public ::testing::Test {
    protected:
        void SetUp() override {}

        NginxConfigParser parser_;
        NginxConfig out_config_;
};

TEST_F(NginxConfigParserTest, SimpleConfig) {
    bool success = parser_.Parse("example_config", &out_config_);
    EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, Comments) {
    bool success = parser_.Parse("pass_test1", &out_config_);
    EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, MultipleServerContexts) {
    bool success = parser_.Parse("pass_test2", &out_config_);
    EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, SingleQuotedStatement) {
    bool success = parser_.Parse("pass_test3", &out_config_);
    EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, EmptyContext) {
    bool success = parser_.Parse("pass_test4", &out_config_);
    EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, NestedContexts) {
    bool success = parser_.Parse("pass_test5", &out_config_);
    EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, EmptyFile) {
    bool success = parser_.Parse("pass_test6", &out_config_);
    EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, StringEscaping) {
    bool success = parser_.Parse("pass_test7", &out_config_);
    EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, CommentOnlyBlockConfig) {
    bool success = parser_.Parse("pass_test8", &out_config_);
    EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, PortParseConfig) {
    bool success = parser_.Parse("pass_test9", &out_config_);
    EXPECT_TRUE(success);

    int port = out_config_.getPort();
    EXPECT_EQ(port, 80);

    std::string repr = out_config_.ToString();
    EXPECT_EQ(repr, "server {\n  port 80;\n}\n");
}

TEST_F(NginxConfigParserTest, HandlerFactoryMapConfig) {
    bool success = parser_.Parse("pass_test10", &out_config_);
    EXPECT_TRUE(success);

    auto handlerFactories = out_config_.getHandlerFactoryMapping();
    EXPECT_TRUE(handlerFactories.find("echo") != handlerFactories.end());
    EXPECT_TRUE(handlerFactories.find("static") != handlerFactories.end());
    EXPECT_TRUE(handlerFactories.find("") != handlerFactories.end());
    EXPECT_TRUE(dynamic_cast<EchoRequestHandlerFactory*>(
                    handlerFactories["echo"].get()) != nullptr);
    EXPECT_TRUE(dynamic_cast<StaticRequestHandlerFactory*>(
                    handlerFactories["static"].get()) != nullptr);
    EXPECT_TRUE(dynamic_cast<CRUDRequestHandlerFactory*>(
                    handlerFactories["api"].get()) != nullptr);
    EXPECT_TRUE(dynamic_cast<SleepRequestHandlerFactory*>(
                    handlerFactories["sleep"].get()) != nullptr);
    EXPECT_TRUE(dynamic_cast<NotFoundHandlerFactory*>(
                    handlerFactories[""].get()) != nullptr);
}

TEST_F(NginxConfigParserTest, NumWorkersPassParseConfig) {
    bool success = parser_.Parse("pass_test11", &out_config_);
    EXPECT_TRUE(success);

    int port = out_config_.getNumWorkers();
    EXPECT_EQ(port, 10);

    std::string repr = out_config_.ToString();
    EXPECT_EQ(repr, "server {\n  num_workers 10;\n}\n");
}

TEST_F(NginxConfigParserTest, NonexistentConfig) {
    bool success = parser_.Parse("does_not_exist", &out_config_);
    EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, MissingClosingQuote) {
    bool success = parser_.Parse("fail_test1", &out_config_);
    EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, MissingSemiColonInNestedStatement) {
    bool success = parser_.Parse("fail_test2", &out_config_);
    EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, ExtraClosingBracket) {
    bool success = parser_.Parse("fail_test3", &out_config_);
    EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, ExtraOpeningBracket) {
    bool success = parser_.Parse("fail_test4", &out_config_);
    EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, MissingClosingBracket) {
    bool success = parser_.Parse("fail_test5", &out_config_);
    EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, MissingWhiteSpaceAfterQuote) {
    bool success = parser_.Parse("fail_test6", &out_config_);
    EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, SemicolonAfterCommentConfig) {
    bool success = parser_.Parse("fail_test7", &out_config_);
    EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, UnmatchedDoubleQuoteConfig) {
    bool success = parser_.Parse("fail_test8", &out_config_);
    EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, NumWorkersFailsParseConfig) {
    bool success = parser_.Parse("fail_test9", &out_config_);
    EXPECT_TRUE(success);
    EXPECT_EQ(out_config_.getNumWorkers(), -1);
}