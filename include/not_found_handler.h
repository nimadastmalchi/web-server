#ifndef NOT_FOUND_HANDLER_H
#define NOT_FOUND_HANDLER_H

#include <map>
#include <string>

#include "config_parser.h"
#include "http_request.h"
#include "request_handler.h"

class NotFoundHandler : public RequestHandler {
    public:
        void handleRequest(const http_request& request,
                           std::string& response) override;
};

#endif