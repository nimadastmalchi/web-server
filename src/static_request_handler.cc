#include "static_request_handler.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include <string>

#include "config_parser.h"
#include "http_request.h"
#include "not_found_handler.h"
#include "request_handler.h"

StaticRequestHandler::StaticRequestHandler(const std::string& path,
                                           const std::string& root)
    : prefix_(path), root_(root) {
    setExtensions();
}

void StaticRequestHandler::setExtensions() {
    extensions_ = {
        {"txt", "text/plain"},      {"htm", "text/html"},
        {"html", "text/html"},      {"jpg", "image/jpeg"},
        {"jpeg", "image/jpeg"},     {"png", "image/png"},
        {"zip", "application/zip"}, {"", "text/plain"},
    };
}

// Format HTTP version response
std::string StaticRequestHandler::createVersionHeader(
    const http_request& request) {
    std::string http_version = "HTTP/" +
                               std::to_string(request.http_version_major) +
                               "." + std::to_string(request.http_version_minor);
    return http_version;
}

// Sets HTTP response string according to parsed parameters from input request.
void StaticRequestHandler::handleRequest(const http_request& request,
                                         std::string& response) {
    // Assumption: server executable run from build directory nested one level
    // deep
    std::string uri = request.uri;
    std::cout << uri << std::endl;
    uri.replace(0, prefix_.length(), "");
    while (!uri.empty() && uri[0] == '/') {
        uri = uri.substr(1);
    }
    std::cout << uri << std::endl;
    if (!root_.empty()) {
        uri = root_ + "/" + uri;
    }
    std::cout << uri << std::endl;

    NginxConfig emptyConfig;
    NotFoundHandler notFoundHandler;

    // Adapted from:
    // https://github.com/JonnyKong/UCLA-CS130-Software-Engineering/blob/master/Assignment4_Static_File_Server/src/request_handler/request_handler_static.cc
    boost::filesystem::path boost_path(uri);
    std::cout << boost::filesystem::absolute(boost_path).string() << std::endl;
    if (!boost::filesystem::exists(uri) ||
        !boost::filesystem::is_regular_file(uri)) {
        notFoundHandler.handleRequest(request, response);
        return;
    }

    // Send 404 response if file not found or can't be opened
    std::ifstream f(uri.c_str(), std::ios::in | std::ios::binary);
    if (!f) {
        notFoundHandler.handleRequest(request, response);
        return;
    }

    // Parse file contents
    std::string body;
    char c;
    while (f.get(c)) body += c;
    f.close();

    std::string http_version = createVersionHeader(request);
    std::string response_code = http_version + " 200 OK\r\n";
    std::string content_length =
        "Content-Length: " + std::to_string(body.length()) + "\r\n";

    std::string extension = "";
    size_t cursor = uri.find_last_of(".");
    if (cursor != std::string::npos) extension = uri.substr(cursor + 1);

    std::string content_type =
        "Content-Type: " + extensions_.at(extension) + "\r\n\r\n";

    response = response_code + content_length + content_type + body + "\r\n";
}
