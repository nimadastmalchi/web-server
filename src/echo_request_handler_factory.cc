#include "echo_request_handler_factory.h"

#include <iostream>
#include <memory>
#include <string>

#include "echo_request_handler.h"
#include "request_handler.h"

EchoRequestHandlerFactory::EchoRequestHandlerFactory(const std::string& path,
                                                     const NginxConfig& config)
    : path_(path), config_(config) {
    parseConfig();
}

std::shared_ptr<RequestHandler> EchoRequestHandlerFactory::createHandler() {
    return std::make_shared<EchoRequestHandler>();
}

void EchoRequestHandlerFactory::parseConfig() {}
