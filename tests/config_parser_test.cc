#include "config_parser.h"
#include "gtest/gtest.h"

class NginxConfigParserTest : public ::testing::Test {
 protected:
  void SetUp() override { }

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

TEST_F(NginxConfigParserTest, NonexistentConfig) {
  bool success = parser_.Parse("does not exist", &out_config_);
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

TEST_F(NginxConfigParserTest, PortParseConfig) {
  bool success = parser_.Parse("pass_test9", &out_config_);
  EXPECT_TRUE(success);

  int port = out_config_.getPort();
  EXPECT_EQ(port, 80);

  std::string repr = out_config_.ToString();
  EXPECT_EQ(repr, "server {\n  port 80;\n}\n");
}