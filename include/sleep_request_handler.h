#ifndef SLEEP_REQUEST_HANDLER_H
#define SLEEP_REQUEST_HANDLER_H

#include <map>
#include <string>

#include "config_parser.h"
#include "request_handler.h"

class SleepRequestHandler : public RequestHandler {
    public:
        SleepRequestHandler(const std::string& path, const int sleep_seconds);

        status handle_request(
            const boost::beast::http::request<boost::beast::http::string_body>&
                request,
            boost::beast::http::response<boost::beast::http::string_body>&
                response) override;

    private:
        int sleep_seconds_;
        std::string prefix_;
};

#endif