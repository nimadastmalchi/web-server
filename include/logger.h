#ifndef LOGGER_H
#define LOGGER_H

#include <memory>

class Logger {
    public: 
        Logger();
        static std::shared_ptr<Logger> logger();
    private:
        static std::shared_ptr<Logger> logger_;
};

#endif