#include "chess_request_handler_factory.h"

#include "chess_request_handler.h"
#include "file_system.h"
#include "request_handler.h"

ChessRequestHandlerFactory::ChessRequestHandlerFactory(
    const std::string& path, const NginxConfig& config)
    : path_(path),
      config_(config),
      file_system_(std::make_shared<FileSystem>()) {
    parseConfig();
}

std::shared_ptr<RequestHandler> ChessRequestHandlerFactory::createHandler() {
    return std::make_shared<ChessRequestHandler>(path_, data_path_, address_,
                                                 file_system_);
}

void ChessRequestHandlerFactory::parseConfig() {
    data_path_ = "";
    for (auto statement : config_.statements_) {
        if (statement->tokens_.size() == 2 &&
            statement->tokens_[0] == "data_path") {
            data_path_ = statement->tokens_[1];
            break;
        }
    }
    while (!data_path_.empty() && data_path_.back() == '/') {
        data_path_.pop_back();
    }
    while (!data_path_.empty() && data_path_.front() == '/') {
        data_path_ = data_path_.substr(1);
    }
}

std::string ChessRequestHandlerFactory::getHandlerName() {
    return "ChessHandler";
}

std::string ChessRequestHandlerFactory::getDataPath() const {
    return data_path_;
}
