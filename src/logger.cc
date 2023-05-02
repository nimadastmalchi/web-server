#include "logger.h"

#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <memory>
#include <string>

using boost::asio::ip::tcp;

void Logger::init_logger(std::string file_name, int rotation_size,
                         Logger::RotationTime rotation_time) {
    // Referenced add_file_log use here:
    // https://stackoverflow.com/questions/39247778/boost-log-how-to-prevent-the-output-will-be-duplicated-to-all-added-streams-whe
    boost::log::register_simple_formatter_factory<
        boost::log::trivial::severity_level, char>("Severity");
    std::string log_format =
        "[%TimeStamp%] (Thread: %ThreadID%) [%Severity%]: %Message%";

    boost::log::add_file_log(
        boost::log::keywords::file_name = file_name,
        boost::log::keywords::rotation_size = rotation_size,
        boost::log::keywords::time_based_rotation =
            boost::log::sinks::file::rotation_at_time_point(
                rotation_time.hour, rotation_time.minute, rotation_time.second),
        boost::log::keywords::format = log_format,
        boost::log::keywords::auto_flush = true);

    boost::log::add_console_log(std::cout,
                                boost::log::keywords::format = log_format);

    boost::log::add_common_attributes();
}

void Logger::log_trace(std::string msg) { BOOST_LOG_TRIVIAL(trace) << msg; }

void Logger::log_debug(std::string msg) { BOOST_LOG_TRIVIAL(debug) << msg; }

void Logger::log_info(std::string msg) { BOOST_LOG_TRIVIAL(info) << msg; }

void Logger::log_warn(std::string msg) { BOOST_LOG_TRIVIAL(warning) << msg; }

void Logger::log_error(std::string msg) { BOOST_LOG_TRIVIAL(error) << msg; }

std::string getClientIdentifier(const tcp::socket& socket) {
    if (!socket.is_open()) return "Closed Socket";
    std::string ip = socket.remote_endpoint().address().to_string();
    std::string port = std::to_string(socket.remote_endpoint().port());
    return ip + ":" + port;
}

void Logger::log_trace(const tcp::socket& socket, std::string msg) {
    BOOST_LOG_TRIVIAL(trace)
        << "[Client: " << getClientIdentifier(socket) << "] " << msg;
}

void Logger::log_debug(const tcp::socket& socket, std::string msg) {
    BOOST_LOG_TRIVIAL(debug)
        << "[Client: " << getClientIdentifier(socket) << "] " << msg;
}

void Logger::log_info(const tcp::socket& socket, std::string msg) {
    BOOST_LOG_TRIVIAL(info)
        << "[Client: " << getClientIdentifier(socket) << "] " << msg;
}

void Logger::log_warn(const tcp::socket& socket, std::string msg) {
    BOOST_LOG_TRIVIAL(warning)
        << "[Client: " << getClientIdentifier(socket) << "] " << msg;
}

void Logger::log_error(const tcp::socket& socket, std::string msg) {
    BOOST_LOG_TRIVIAL(error)
        << "[Client: " << getClientIdentifier(socket) << "] " << msg;
}
