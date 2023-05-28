#include "not_found_handler_factory.h"

#include <iostream>
#include <memory>
#include <string>

#include "not_found_handler.h"
#include "request_handler.h"

NotFoundHandlerFactory::NotFoundHandlerFactory(const std::string& path,
                                               const NginxConfig& config)
    : path_(path), config_(config) {
    parseConfig();
}

std::shared_ptr<RequestHandler> NotFoundHandlerFactory::createHandler() {
    return std::make_shared<NotFoundHandler>();
}

void NotFoundHandlerFactory::parseConfig() {}

std::string NotFoundHandlerFactory::getHandlerName() {
    return "NotFoundHandler";
}
