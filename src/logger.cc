#include "logger.h"

#include <memory>

std::shared_ptr<Logger> Logger::logger_(nullptr);

Logger::Logger() {}

std::shared_ptr<Logger> Logger::logger() {
    if (logger_ == nullptr) logger_.reset(new Logger());
    return logger_;
}