#include "chess_request_handler_factory.h"

#include "config_parser.h"
#include "gtest/gtest.h"

class ChessRequestHandlerFactoryTest : public ::testing::Test {
    protected:
        void SetUp() override {
            parser_.Parse("example_location_config", &config_);
        }

        NginxConfigParser parser_;
        NginxConfig config_;
};

TEST_F(ChessRequestHandlerFactoryTest, ParseConfig) {
    ChessRequestHandlerFactory handlerFactory("chess", config_);

    EXPECT_EQ(handlerFactory.getDataPath(), "crud_test");
}
