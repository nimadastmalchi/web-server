#include "chess_request_handler.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "logger.h"
#include "not_found_handler.h"

namespace http = boost::beast::http;

ChessRequestHandler::ChessRequestHandler(
    const std::string& path, const std::string& data_path,
    std::shared_ptr<FileSystem> file_system)
    : path_(path), data_path_(data_path), file_system_(file_system) {}

// Helper function to extract ID suffix from full file path:
std::string get_suffix(std::string request_str) {
    std::string curr_symbol;
    for (size_t i = 0; i < request_str.length(); i++) {
        if (request_str[i] == '/') {
            curr_symbol = "";
        } else {
            curr_symbol += request_str[i];
        }
    }

    return curr_symbol;
}

std::vector<std::string> ChessRequestHandler::parse_request_path(
    std::string request_str) {
    while (request_str.size() > 0 && request_str.back() == '/')
        request_str.pop_back();
    std::string shortened_request = request_str.substr(path_.size());
    std::vector<std::string> parsed;

    if (shortened_request.size() > 0) {
        std::string curr_symbol;
        for (size_t i = 1; i < shortened_request.length(); i++) {
            if (shortened_request[i] == '/') {
                parsed.push_back(curr_symbol);
                curr_symbol = "";
            } else {
                curr_symbol += shortened_request[i];
            }
        }
        parsed.push_back(curr_symbol);
    }

    return parsed;
}

status ChessRequestHandler::handle_get(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response) {
    return true;
}

status ChessRequestHandler::handle_create(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response) {
    return true;
}

status ChessRequestHandler::handle_put(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response) {
    return true;
}

status ChessRequestHandler::handle_request(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response) {
    Logger::log_trace("Chess request");

    switch (request.method()) {
        case http::verb::get:
            Logger::log_trace("GET");
            return handle_get(request, response);
        case http::verb::post:
            Logger::log_trace("POST");
            return handle_create(request, response);
        case http::verb::put:
            Logger::log_trace("PUT");
            return handle_put(request, response);
        default:
            Logger::log_trace("UNKNOWN");
            response.version(request.version());
            response.result(http::status::method_not_allowed);
            response.set(http::field::content_type, "text/plain");
            response.body() = "";
            response.prepare_payload();
    }

    return true;
}
