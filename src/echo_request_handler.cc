#include "echo_request_handler.h"

#include <string>

#include "http_request.h"

void EchoRequestHandler::handleRequest(const http_request& request,
                                       std::string& response) {
    std::string http_version = "HTTP/" +
                               std::to_string(request.http_version_major) +
                               "." + std::to_string(request.http_version_minor);
    std::string response_code = http_version + " 200 0K\r\n";
    std::string content_type = "Content-Type: text/plain\r\n\r\n";

    std::string content = request.raw + "\r\n";
    response = response_code + content_type + content + "\r\n";
}
