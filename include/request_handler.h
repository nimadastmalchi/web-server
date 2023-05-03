#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <iostream>
#include <string>

class http_request;

class RequestHandler {
    public:
        virtual void handleRequest(const http_request &request_,
                                   std::string &response) = 0;
};

#endif