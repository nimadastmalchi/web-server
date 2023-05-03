#ifndef STATIC_REQUEST_HANDLER_H
#define STATIC_REQUEST_HANDLER_H

#include <map>
#include <string>

#include "request_handler.h"

class http_request;

class StaticRequestHandler : public RequestHandler {
    public:
        StaticRequestHandler(std::string root, std::string prefix);
        void handleRequest(const http_request& request,
                           std::string& response) override;

    private:
        std::string createVersionHeader(const http_request& request);
        std::string create404Response(const http_request& request);
        std::string root_;
        std::string prefix_;
        std::map<std::string, std::string> extensions_;
};

#endif