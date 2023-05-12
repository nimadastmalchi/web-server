/**
 * Request handler for echo.
 */

#ifndef REQUEST_HANDLER_ECHO_H
#define REQUEST_HANDLER_ECHO_H

#include <iostream>

#include "config_parser.h"
#include "request_handler.h"

class EchoRequestHandler : public RequestHandler {
    public:
        void handleRequest(const http_request& request,
                           std::string& response) override;
};

#endif  // REQUEST_HANDLER_ECHO_H