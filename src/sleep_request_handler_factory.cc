#include "sleep_request_handler_factory.h"

#include <iostream>
#include <memory>
#include <string>

#include "config_parser.h"
#include "request_handler.h"
#include "sleep_request_handler.h"

SleepRequestHandlerFactory::SleepRequestHandlerFactory(
    const std::string& path, const NginxConfig& config)
    : path_(path), config_(config) {
    parseConfig();
}

std::shared_ptr<RequestHandler> SleepRequestHandlerFactory::createHandler() {
    return std::make_shared<SleepRequestHandler>(path_, sleep_seconds_);
}

void SleepRequestHandlerFactory::parseConfig() {
    sleep_seconds_ = DEFAULT_MAX_SLEEP_SECONDS_;
    for (auto statement : config_.statements_) {
        if (statement->tokens_.size() == 2 &&
            statement->tokens_[0] == "sleep_seconds") {
            std::string val = statement->tokens_[1];
            bool is_number = true;
            for (auto c : val) {
                if (!std::isdigit(c)) {
                    is_number = false;
                    break;
                }
            }
            if (is_number) sleep_seconds_ = std::stoi(val);
            break;
        }
    }
}

int SleepRequestHandlerFactory::getSleepSeconds() { return sleep_seconds_; }
