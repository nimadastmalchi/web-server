#include "crud_request_handler_factory.h"

#include "crud_request_handler.h"
#include "file_system.h"
#include "request_handler.h"

CRUDRequestHandlerFactory::CRUDRequestHandlerFactory(const std::string& path,
                                                     const NginxConfig& config)
    : path_(path),
      config_(config),
      file_system_(std::make_shared<FileSystem>()) {
    parseConfig();
}

std::shared_ptr<RequestHandler> CRUDRequestHandlerFactory::createHandler() {
    return std::make_shared<CRUDRequestHandler>(path_, root_, file_system_);
}

void CRUDRequestHandlerFactory::parseConfig() {
    root_ = "";
    for (auto statement : config_.statements_) {
        if (statement->tokens_.size() == 2 &&
            statement->tokens_[0] == "data_path") {
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

std::string CRUDRequestHandlerFactory::getHandlerName() {
    return "CRUDHandler";
}

std::string CRUDRequestHandlerFactory::getRoot() { return root_; }
