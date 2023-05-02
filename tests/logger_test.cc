#include "logger.h"

#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <boost/thread/thread.hpp>
#include <filesystem>
#include <regex>

#include "gtest/gtest.h"

using boost::asio::ip::tcp;

class LoggerTest : public ::testing::Test {
    protected:
        void SetUp() override {
            // 10 * 1024 * 1024 = 10MB rotation size
            // 0, 0, 0 = 12:00:00 rotation time
            Logger::init_logger("test.log", 10 * 1024 * 1024, {0, 0, 0});
            testing::internal::CaptureStdout();
        }

        void TearDown() { std::filesystem::remove("test.log"); }
};

struct LogSplit {
        std::string timestamp;
        std::string thread_id;
        std::string sev;
        std::string client_info;
        std::string msg;
};

LogSplit split_log(std::string log) {
    boost::regex log_regex{
        "\\[(.+)\\] \\(Thread: (.+)\\) \\[(.*)\\]:(?: \\[Client: (.*)\\])? "
        "(.+)\n"};
    boost::smatch match;
    boost::regex_match(log, match, log_regex);
    return LogSplit{match[1], match[2], match[3], match[4], match[5]};
}

TEST_F(LoggerTest, LogTrace) {
    std::string msg = "Test";
    Logger::log_trace(msg);
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << output << std::endl;
    auto log_components = split_log(output);
    EXPECT_EQ(log_components.msg, msg);
    EXPECT_EQ(log_components.sev, "trace");
}

TEST_F(LoggerTest, LogDebug) {
    std::string msg = "Test";
    Logger::log_debug(msg);
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << output << std::endl;
    auto log_components = split_log(output);
    EXPECT_EQ(log_components.msg, msg);
    EXPECT_EQ(log_components.sev, "debug");
}

TEST_F(LoggerTest, LogInfo) {
    std::string msg = "Test";
    Logger::log_info(msg);
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << output << std::endl;
    auto log_components = split_log(output);
    EXPECT_EQ(log_components.msg, msg);
    EXPECT_EQ(log_components.sev, "info");
}

TEST_F(LoggerTest, LogWarn) {
    std::string msg = "Test";
    Logger::log_warn(msg);
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << output << std::endl;
    auto log_components = split_log(output);
    EXPECT_EQ(log_components.msg, msg);
    EXPECT_EQ(log_components.sev, "warning");
}

TEST_F(LoggerTest, LogError) {
    std::string msg = "Test";
    Logger::log_error(msg);
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << output << std::endl;
    auto log_components = split_log(output);
    EXPECT_EQ(log_components.msg, msg);
    EXPECT_EQ(log_components.sev, "error");
}

TEST_F(LoggerTest, LogTraceSocket) {
    std::string msg = "Test";
    boost::asio::io_service io_service;
    Logger::log_trace(tcp::socket(io_service), msg);
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << output << std::endl;
    auto log_components = split_log(output);
    EXPECT_EQ(log_components.msg, msg);
    EXPECT_EQ(log_components.sev, "trace");
    EXPECT_EQ(log_components.client_info, "Closed Socket");
}

TEST_F(LoggerTest, LogDebugSocket) {
    std::string msg = "Test";
    boost::asio::io_service io_service;
    Logger::log_debug(tcp::socket(io_service), msg);
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << output << std::endl;
    auto log_components = split_log(output);
    EXPECT_EQ(log_components.msg, msg);
    EXPECT_EQ(log_components.sev, "debug");
    EXPECT_EQ(log_components.client_info, "Closed Socket");
}

TEST_F(LoggerTest, LogInfoSocket) {
    std::string msg = "Test";
    boost::asio::io_service io_service;
    Logger::log_info(tcp::socket(io_service), msg);
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << output << std::endl;
    auto log_components = split_log(output);
    EXPECT_EQ(log_components.msg, msg);
    EXPECT_EQ(log_components.sev, "info");
    EXPECT_EQ(log_components.client_info, "Closed Socket");
}

TEST_F(LoggerTest, LogWarnSocket) {
    std::string msg = "Test";
    boost::asio::io_service io_service;
    Logger::log_warn(tcp::socket(io_service), msg);
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << output << std::endl;
    auto log_components = split_log(output);
    EXPECT_EQ(log_components.msg, msg);
    EXPECT_EQ(log_components.sev, "warning");
    EXPECT_EQ(log_components.client_info, "Closed Socket");
}

TEST_F(LoggerTest, LogErrorSocket) {
    std::string msg = "Test";
    boost::asio::io_service io_service;
    Logger::log_error(tcp::socket(io_service), msg);
    std::string output = testing::internal::GetCapturedStdout();
    std::cout << output << std::endl;
    auto log_components = split_log(output);
    EXPECT_EQ(log_components.msg, msg);
    EXPECT_EQ(log_components.sev, "error");
    EXPECT_EQ(log_components.client_info, "Closed Socket");
}