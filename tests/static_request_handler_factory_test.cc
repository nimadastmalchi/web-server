#include "static_request_handler_factory.h"

#include "config_parser.h"
#include "gtest/gtest.h"

class StaticRequestHandlerFactoryTest : public ::testing::Test {
    protected:
        void SetUp() override {
            parser_.Parse("example_location_config", &config_);
        }

        NginxConfigParser parser_;
        NginxConfig config_;
};

TEST_F(StaticRequestHandlerFactoryTest, ParseConfig) {
    StaticRequestHandlerFactory handlerFactory("static", config_);

    EXPECT_EQ(handlerFactory.getRoot(), "test");
}
