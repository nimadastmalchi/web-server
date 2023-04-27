#include "logger.h"
#include "gtest/gtest.h"

class LoggerTest : public ::testing::Test {
  protected:
    void SetUp() override { }
};

TEST_F(LoggerTest, SingletonLoggerCreated) {
    EXPECT_EQ(Logger::logger(), Logger::logger());
}