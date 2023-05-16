#include "not_found_handler.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include <string>

#include "config_parser.h"
#include "request_handler.h"

using namespace boost::beast;

status NotFoundHandler::handle_request(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response) {
    std::string content =
        "<html><head><title>404 Not Found</title></head><body><p>404 Not "
        "Found</p></body></html>";

    response.version(request.version());
    response.result(http::status::not_found);
    response.set(http::field::content_type, "text/html");
    response.body() = content;
    response.prepare_payload();
    return true;
}