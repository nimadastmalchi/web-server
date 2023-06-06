#include "chess_request_handler.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "logger.h"
#include "not_found_handler.h"

namespace http = boost::beast::http;

ChessRequestHandler::ChessRequestHandler(
    const std::string& path, const std::string& data_path,
    const std::string& address, std::shared_ptr<FileSystem> file_system)
    : path_(path),
      data_path_(data_path),
      address_(address),
      file_system_(file_system) {}

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

std::vector<std::string> parse_file_body(std::string body) {
    std::vector<std::string> parsed;
    std::string curr_symbol = "";

    for (size_t i = 0; i < body.size(); i++) {
        if (body[i] == '\n') {
            parsed.push_back(curr_symbol);
            curr_symbol = "";
        } else {
            curr_symbol += body[i];
        }
    }

    return parsed;
}

void populate_bad_response(const http::request<http::string_body>& request,
                           http::response<http::string_body>& response) {
    response.version(request.version());
    response.result(http::status::bad_request);
    response.set(boost::beast::http::field::content_type, "text/plain");
    response.body() = "";
    response.prepare_payload();
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
    std::string request_str(request.target().data(), request.target().size());
    std::vector<std::string> parsed_path = parse_request_path(request_str);

    int path_length = parsed_path.size();
    if (path_length == 0) {
        // Handle GET /chess:
        // TODO...
        return true;
    } else if (path_length == 1) {
        // Handle GET /chess/<id>:
        std::string file_path = data_path_ + "/" + parsed_path[0];
        // If the ID exists, return chessboard.html:
        if (file_system_->read_file(file_path)) {
            auto contents =
                file_system_->read_file("/chess130/chessboard.html");
            if (contents) {
                response.body() = contents.value();
                response.version(request.version());
                response.result(http::status::ok);
                response.set(http::field::content_type, "text/html");
                response.prepare_payload();

                return true;
            }
        }
        // If the file doesn't exist, return 404:
        NotFoundHandler not_found_handler;
        return not_found_handler.handle_request(request, response);
    } else if (path_length == 2) {
        // Handle GET /chess/games/<id>:
        // Unsupported endpoint:
        if (parsed_path[0] != "games") {
            Logger::log_trace("Bad Chess GET request");
            populate_bad_response(request, response);
            return false;
        }

        std::string file_path = data_path_ + "/" + parsed_path[1];
        auto contents = file_system_->read_file(file_path);
        if (contents) {
            std::string file_body = contents.value();
            std::vector<std::string> parsed_file_body =
                parse_file_body(file_body);

            if (parsed_file_body.size() != 3) {
                // If the file is incorrectly formatted, delete it and return
                // 404:
                Logger::log_trace("Deleting incorrectly formatted file");
                file_system_->delete_file(file_path);
                NotFoundHandler not_found_handler;
                return not_found_handler.handle_request(request, response);
            }

            std::string role;
            std::string updated_file_body;
            std::string white_address = parsed_file_body[0];
            std::string black_address = parsed_file_body[1];
            std::string fen = parsed_file_body[2];
            if (white_address.empty()) {
                role = "w";
                updated_file_body =
                    address_ + "\n" + black_address + "\n" + fen + "\n";
            } else if (black_address.empty()) {
                role = "b";
                updated_file_body =
                    white_address + "\n" + address_ + "\n" + fen + "\n";
            } else {
                if (address_ == white_address) {
                    role = "w";
                } else if (address_ == black_address) {
                    role = "b";
                } else {
                    role = "v";
                }
                updated_file_body = file_body;
            }
            file_system_->write_file(file_path, updated_file_body);

            std::string response_body =
                "{\"role\": \"" + role + "\", \"fen\": \"" + fen + "\"}";
            response.body() = response_body;
            response.version(request.version());
            response.result(http::status::ok);
            response.set(boost::beast::http::field::content_type,
                         "application/json");
            response.prepare_payload();

            return true;
        }
        // If the file doesn't exist, return 404:
        NotFoundHandler not_found_handler;
        return not_found_handler.handle_request(request, response);
    } else {
        // Bad request:
        Logger::log_trace("Bad Chess GET request");
        populate_bad_response(request, response);
        return false;
    }

    return true;
}

// Handle POST /chess/new:
status ChessRequestHandler::handle_create(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response) {
    std::string request_str(request.target().data(), request.target().size());
    std::string operation = request_str.substr(path_.size());

    if (operation != "/new") {
        Logger::log_trace("Bad Chess POST request");
        populate_bad_response(request, response);
        return false;
    }

    // Generate a pseudo-random 5-digit ID:
    std::srand((unsigned)std::time(NULL));
    std::string new_id = std::to_string(std::rand() % 100000);
    // Don't left-pad with zeros:
    std::string file_path = data_path_ + "/" + new_id;
    // Check that the ID doesn't already exist:
    while (file_system_->read_file(file_path)) {
        new_id = std::to_string(std::rand() % 100000);
        file_path = data_path_ + "/" + new_id;
    }

    // Write the starting state of the game:
    std::string file_body = "\n\nrnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR\n";
    file_system_->write_file(file_path, file_body);

    // Return the ID of the game:
    http::status response_code = http::status::created;
    std::string content_type = "application/json";
    std::string response_body = "{\"id\": " + new_id + "}";

    response.version(request.version());
    response.result(response_code);
    response.set(boost::beast::http::field::content_type, content_type);
    response.body() = response_body;
    response.prepare_payload();

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
