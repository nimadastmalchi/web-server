#include "static_request_handler.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include <string>

#include "config_parser.h"
#include "not_found_handler.h"
#include "request_handler.h"

using namespace boost::beast;

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

// Sets HTTP response string according to parsed parameters from input request.
status StaticRequestHandler::handle_request(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response) {
    std::string uri = request.target().to_string();
    uri.replace(0, prefix_.length(), "");
    while (!uri.empty() && uri[0] == '/') {
        uri = uri.substr(1);
    }
    if (!root_.empty()) {
        uri = root_ + "/" + uri;
    }

    NotFoundHandler notFoundHandler;

    // Adapted from:
    // https://github.com/JonnyKong/UCLA-CS130-Software-Engineering/blob/master/Assignment4_Static_File_Server/src/request_handler/request_handler_static.cc
    boost::filesystem::path boost_path(uri);
    if (!boost::filesystem::exists(uri) ||
        !boost::filesystem::is_regular_file(uri)) {
        notFoundHandler.handle_request(request, response);
        return false;
    }

    // Send 404 response if file not found or can't be opened
    std::ifstream f(uri.c_str(), std::ios::in | std::ios::binary);
    if (!f) {
        notFoundHandler.handle_request(request, response);
        return false;
    }

    // Parse file contents
    std::string body;
    char c;
    while (f.get(c)) body += c;
    f.close();

    response.result(http::status::ok);
    response.version(request.version());
    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(body.length()));
    response.body() = std::move(body);    

    response.prepare_payload();

    return true;
}
