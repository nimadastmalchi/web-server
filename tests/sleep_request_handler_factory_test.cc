#include "sleep_request_handler_factory.h"

#include "config_parser.h"
#include "gtest/gtest.h"

class SleepRequestHandlerFactoryTest : public ::testing::Test {
    protected:
        void SetUp() override {
            parser_.Parse("example_location_config", &config_);
        }

        NginxConfigParser parser_;
        NginxConfig config_;
};

TEST_F(SleepRequestHandlerFactoryTest, ParseConfig) {
    SleepRequestHandlerFactory handlerFactory("sleep_seconds", config_);

    EXPECT_EQ(handlerFactory.getSleepSeconds(), 30);
}
