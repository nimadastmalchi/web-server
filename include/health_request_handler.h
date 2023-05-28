#ifndef HEALTH_REQUEST_HANDLER_H
#define HEALTH_REQUEST_HANDLER_H

#include <iostream>

#include "config_parser.h"
#include "request_handler.h"

class HealthRequestHandler : public RequestHandler {
    public:
        status handle_request(
            const boost::beast::http::request<boost::beast::http::string_body>&
                request,
            boost::beast::http::response<boost::beast::http::string_body>&
                response) override;
};

#endif