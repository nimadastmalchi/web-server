#ifndef STATIC_REQUEST_HANDLER_H
#define STATIC_REQUEST_HANDLER_H

#include <map>
#include <string>

#include "config_parser.h"
#include "request_handler.h"

class StaticRequestHandler : public RequestHandler {
    public:
        StaticRequestHandler(const std::string& path, const std::string& root);

        status handle_request(
            const boost::beast::http::request<boost::beast::http::string_body>&
                request,
            boost::beast::http::response<boost::beast::http::string_body>&
                response) override;

    private:
        void setExtensions();
        std::string root_;
        std::string prefix_;
        std::map<std::string, std::string> extensions_;
};

#endif