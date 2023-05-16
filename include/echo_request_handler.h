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
        status handle_request(
            const boost::beast::http::request<boost::beast::http::string_body>&
                request,
            boost::beast::http::response<boost::beast::http::string_body>&
                response) override;
};

#endif  // REQUEST_HANDLER_ECHO_H