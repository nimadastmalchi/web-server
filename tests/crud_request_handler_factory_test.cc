#include "crud_request_handler_factory.h"

#include "config_parser.h"
#include "gtest/gtest.h"

class CrudRequestHandlerFactoryTest : public ::testing::Test {
    protected:
        void SetUp() override {
            parser_.Parse("example_location_config", &config_);
        }

        NginxConfigParser parser_;
        NginxConfig config_;
};

TEST_F(CrudRequestHandlerFactoryTest, ParseConfig) {
    CRUDRequestHandlerFactory handlerFactory("crud", config_);

    EXPECT_EQ(handlerFactory.getRoot(), "path_test");
}
