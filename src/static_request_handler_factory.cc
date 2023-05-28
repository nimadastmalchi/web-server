#include "static_request_handler_factory.h"

#include <iostream>
#include <memory>
#include <string>

#include "config_parser.h"
#include "request_handler.h"
#include "static_request_handler.h"

StaticRequestHandlerFactory::StaticRequestHandlerFactory(
    const std::string& path, const NginxConfig& config)
    : path_(path), config_(config) {
    parseConfig();
}

std::shared_ptr<RequestHandler> StaticRequestHandlerFactory::createHandler() {
    return std::make_shared<StaticRequestHandler>(path_, root_);
}

void StaticRequestHandlerFactory::parseConfig() {
    root_ = "";
    for (auto statement : config_.statements_) {
        if (statement->tokens_.size() == 2 && statement->tokens_[0] == "root") {
            root_ = statement->tokens_[1];
            break;
        }
    }
    while (!root_.empty() && root_.back() == '/') {
        root_.pop_back();
    }
    while (!root_.empty() && root_.front() == '/') {
        root_ = root_.substr(1);
    }
}

std::string StaticRequestHandlerFactory::getHandlerName() {
    return "StaticHandler";
}

std::string StaticRequestHandlerFactory::getRoot() { return root_; }
