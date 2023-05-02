#include "logger.h"

#include <boost/log/core.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

#include <memory>

std::shared_ptr<Logger> Logger::logger_(nullptr);

Logger::Logger() {}

std::shared_ptr<Logger> Logger::logger() {
    if (logger_ == nullptr) logger_.reset(new Logger());
    return logger_;
}

void Logger::init(std::string file_name, int rotation_size, Logger::RotationTime rotation_time) {
    // Referenced add_file_log used here: https://stackoverflow.com/questions/39247778/boost-log-how-to-prevent-the-output-will-be-duplicated-to-all-added-streams-whe
    boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");
    std::string log_format = "[%TimeStamp%] (Thread: %ThreadID%) [%Severity%]: %Message%";
   
    boost::log::add_file_log(
        boost::log::keywords::file_name = file_name,
        boost::log::keywords::rotation_size = rotation_size,
        boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(rotation_time.hour, rotation_time.minute, rotation_time.second),
        boost::log::keywords::format = log_format,
        boost::log::keywords::auto_flush = true
    );

    boost::log::add_console_log(
        std::cout,
        boost::log::keywords::format = log_format
    );

    boost::log::add_common_attributes();
}

void Logger::log_trace(std::string msg) {
    BOOST_LOG_TRIVIAL(trace) << msg;
}

void Logger::log_debug(std::string msg) {
    BOOST_LOG_TRIVIAL(debug) << msg;
}

void Logger::log_info(std::string msg) {
    BOOST_LOG_TRIVIAL(info) << msg;
}

void Logger::log_warn(std::string msg) {
    BOOST_LOG_TRIVIAL(warning) << msg;
}

void Logger::log_error(std::string msg) {
    BOOST_LOG_TRIVIAL(error) << msg;
}
