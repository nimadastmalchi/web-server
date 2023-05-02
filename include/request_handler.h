#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <iostream>
#include <string>

#include "http_request.h"

class RequestHandler {
    public:
        virtual void handleRequest(const http_request &request_,
                                   std::string &response) = 0;
};

#endif