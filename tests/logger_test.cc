#include "logger.h"
#include "gtest/gtest.h"

#include <boost/regex.hpp>
#include <boost/thread/thread.hpp>
#include <filesystem>
#include <regex>

class LoggerTest : public ::testing::Test {
  protected:
    void SetUp() override { 
      	// 10 * 1024 * 1024 = 10MB rotation size
      	// 0, 0, 0 = 12:00:00 rotation time
        Logger::logger()->init("test.log", 10 * 1024 * 1024, {
            0, 0, 0
        });
        testing::internal::CaptureStdout();
    }

    void TearDown() {
        std::filesystem::remove("test.log");
    }
};

struct LogSplit {
    std::string timestamp;
    std::string thread_id;
    std::string sev;
    std::string msg;
};

LogSplit split_log(std::string log) {
    boost::regex log_regex {"\\[(.+)\\] \\(Thread: (.+)\\) \\[(.*)\\]: (.+)\n"};
    boost::smatch match;
    boost::regex_match(log, match, log_regex);
    return LogSplit {
       match[1],
       match[2],
       match[3],
       match[4]
    };
}

TEST_F(LoggerTest, SingletonLoggerCreated) {
    EXPECT_EQ(Logger::logger(), Logger::logger());
}

TEST_F(LoggerTest, LogTrace) {
    std::string msg = "Test";
    Logger::logger()->log_trace(msg);
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << output << std::endl;
    auto log_components = split_log(output);
    EXPECT_EQ(log_components.msg, msg);
    EXPECT_EQ(log_components.sev, "trace");
}

TEST_F(LoggerTest, LogDebug) {
    std::string msg = "Test";
    Logger::logger()->log_debug(msg);
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << output << std::endl;
    auto log_components = split_log(output);
    EXPECT_EQ(log_components.msg, msg);
    EXPECT_EQ(log_components.sev, "debug");
}

TEST_F(LoggerTest, LogInfo) {
    std::string msg = "Test";
    Logger::logger()->log_info(msg);
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << output << std::endl;
    auto log_components = split_log(output);
    EXPECT_EQ(log_components.msg, msg);
    EXPECT_EQ(log_components.sev, "info");
}

TEST_F(LoggerTest, LogWarn) {
    std::string msg = "Test";
    Logger::logger()->log_warn(msg);
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << output << std::endl;
    auto log_components = split_log(output);
    EXPECT_EQ(log_components.msg, msg);
    EXPECT_EQ(log_components.sev, "warning");
}

TEST_F(LoggerTest, LogError) {
    std::string msg = "Test";
    Logger::logger()->log_error(msg);
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << output << std::endl;
    auto log_components = split_log(output);
    EXPECT_EQ(log_components.msg, msg);
    EXPECT_EQ(log_components.sev, "error");
}
