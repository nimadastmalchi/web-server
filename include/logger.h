#ifndef LOGGER_H
#define LOGGER_H

#include <boost/asio.hpp>
#include <memory>

using boost::asio::ip::tcp;

class Logger {
    public:
        struct RotationTime {
                int hour;
                int minute;
                int second;

                RotationTime(int hour, int minute, int second)
                    : hour(hour), minute(minute), second(second) {}
        };

        static void init_logger(std::string file_name, int rotation_size,
                                RotationTime rotation_time);
        static void log_trace(std::string msg);
        static void log_debug(std::string msg);
        static void log_info(std::string msg);
        static void log_warn(std::string msg);
        static void log_error(std::string msg);

        static void log_trace(const tcp::socket& socket, std::string msg);
        static void log_debug(const tcp::socket& socket, std::string msg);
        static void log_info(const tcp::socket& socket, std::string msg);
        static void log_warn(const tcp::socket& socket, std::string msg);
        static void log_error(const tcp::socket& socket, std::string msg);
};

#endif