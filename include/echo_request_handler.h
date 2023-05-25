#ifndef ECHO_REQUEST_HANDLER_H
#define ECHO_REQUEST_HANDLER_H

#include <iostream>

#include "config_parser.h"
#include "request_handler.h"

class EchoRequestHandler : public RequestHandler {
    public:
        status handle_request(
            const boost::beast::http::request<boost::beast::http::string_body>&
                request,
            boost::beast::http::response<boost::beast::http::string_body>&
                response) override;
};

#endif