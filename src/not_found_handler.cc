#include "not_found_handler.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include <string>

#include "config_parser.h"
#include "http_request.h"
#include "request_handler.h"

void NotFoundHandler::handleRequest(const http_request& request,
                                    std::string& response) {
    std::string http_version = "HTTP/" +
                               std::to_string(request.http_version_major) +
                               "." + std::to_string(request.http_version_minor);
    std::string response_code = http_version + " 404 Not Found\r\n";
    std::string content_type = "Content-Type: text/html\r\n\r\n";
    std::string content =
        "<html><head><title>404 Not Found</title></head><body><p>404 Not "
        "Found</p></body></html>";
    response = response_code + content_type + content + "\r\n";
}
