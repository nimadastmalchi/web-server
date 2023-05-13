#ifndef STATIC_REQUEST_HANDLER_H
#define STATIC_REQUEST_HANDLER_H

#include <map>
#include <string>

#include "config_parser.h"
#include "http_request.h"
#include "request_handler.h"

class StaticRequestHandler : public RequestHandler {
    public:
        StaticRequestHandler(const std::string& path, const std::string& root);
        void handleRequest(const http_request& request,
                           std::string& response) override;

    private:
        void setExtensions();
        std::string createVersionHeader(const http_request& request);
        std::string root_;
        std::string prefix_;
        std::map<std::string, std::string> extensions_;
};

#endif