#ifndef NOT_FOUND_HANDLER_H
#define NOT_FOUND_HANDLER_H

#include <map>
#include <string>

#include "config_parser.h"
#include "request_handler.h"

class NotFoundHandler : public RequestHandler {
    public:
        status handle_request(
            const boost::beast::http::request<boost::beast::http::string_body>&
                request,
            boost::beast::http::response<boost::beast::http::string_body>&
                response) override;
};

#endif