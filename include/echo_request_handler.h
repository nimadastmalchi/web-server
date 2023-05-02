/**
 * Request handler for echo.
 */

#ifndef REQUEST_HANDLER_ECHO_H
#define REQUEST_HANDLER_ECHO_H

#include <iostream>

#include "request_handler.h"

class EchoRequestHandler : public RequestHandler {
    public:
        EchoRequestHandler() {}
        void handleRequest(const http_request &request_, std::string &response);
};

#endif  // REQUEST_HANDLER_ECHO_H