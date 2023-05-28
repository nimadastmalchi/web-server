#include "health_request_handler_factory.h"

#include <iostream>
#include <memory>
#include <string>

#include "health_request_handler.h"
#include "request_handler.h"

HealthRequestHandlerFactory::HealthRequestHandlerFactory(
    const std::string& path, const NginxConfig& config)
    : path_(path), config_(config) {
    parseConfig();
}

std::shared_ptr<RequestHandler> HealthRequestHandlerFactory::createHandler() {
    return std::make_shared<HealthRequestHandler>();
}

void HealthRequestHandlerFactory::parseConfig() {}
