#ifndef LOGGER_H
#define LOGGER_H

#include <memory>

class Logger {
    public: 

        struct RotationTime {
            int hour;
            int minute;
            int second;

            RotationTime(int hour, int minute, int second): hour(hour), minute(minute), second(second) { }
        };

        Logger();
        void init(std::string file_name, int rotation_size, RotationTime rotation_time);
        void log_trace(std::string msg);
        void log_debug(std::string msg);
        void log_info(std::string msg);
        void log_warn(std::string msg);
        void log_error(std::string msg);
        static std::shared_ptr<Logger> logger();
  
    private:
  
        static std::shared_ptr<Logger> logger_;
};

#endif