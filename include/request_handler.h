#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <boost/beast/http.hpp>
#include <iostream>
#include <string>

typedef bool status;

class RequestHandler {
    public:
        virtual status handle_request(
            const boost::beast::http::request<boost::beast::http::string_body>&
                request,
            boost::beast::http::response<boost::beast::http::string_body>&
                response) = 0;
};

#endif