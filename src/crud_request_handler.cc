#include "crud_request_handler.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "logger.h"
#include "not_found_handler.h"

namespace http = boost::beast::http;

CRUDRequestHandler::CRUDRequestHandler(const std::string& path,
                                       const std::string& data_path,
                                       std::shared_ptr<FileSystem> file_system)
    : path_(path), data_path_(data_path), file_system_(file_system) {}

// Helper function to extract ID suffix from full file path:
static std::string get_suffix(std::string request_str) {
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

std::vector<std::string> CRUDRequestHandler::parse_request_path(
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

status CRUDRequestHandler::handle_get(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response) {
    std::string request_str(request.target().data(), request.target().size());
    std::vector<std::string> parsed = parse_request_path(request_str);

    // Bad request:
    if (parsed.size() == 0) {
        Logger::log_trace("Bad CRUD GET request");
        response.version(request.version());
        response.result(http::status::bad_request);
        response.set(boost::beast::http::field::content_type, "text/plain");
        response.body() = "";
        response.prepare_payload();

        return false;
    }

    // Assumption: Request is a list operation if last token is not a number.
    std::string last = parsed[parsed.size() - 1];
    bool is_list = false;
    for (int i = 0; i < last.size(); i++) {
        if (!isdigit(last[i])) is_list = true;
    }

    // Return directory list in response body:
    if (is_list) {
        std::string directory_path = data_path_;
        for (int i = 0; i < parsed.size(); i++) {
            directory_path = directory_path + "/" + parsed[i];
        }

        Logger::log_trace("Listing entity directory: " + directory_path);
        auto files = file_system_->list_files(directory_path);
        if (files) {
            std::string ids;

            for (auto file_name : files.value()) {
                std::string id = get_suffix(file_name);
                ids = ids + id + ", ";
            }

            // Remove trailing ", " characters:
            if (ids.size() >= 2) {
                ids = ids.substr(0, ids.size() - 2);
            }

            std::string list = "[" + ids + "]";
            response.body() = list;
            response.version(request.version());
            response.result(http::status::ok);
            response.set(boost::beast::http::field::content_type,
                         "application/json");
            response.prepare_payload();

            return true;
        }
    } else if (parsed.size() > 1) {
        // Assumption: All entity files will be named their integer ID.
        std::string entity_name;
        for (int i = 0; i < parsed.size() - 1; i++) {
            entity_name = entity_name + "/" + parsed[i];
        }

        std::string entity_number = parsed[parsed.size() - 1];
        std::string file_path = data_path_ + entity_name + "/" + entity_number;

        Logger::log_trace("Retrieving entity at: " + file_path);
        auto contents = file_system_->read_file(file_path);
        if (contents) {
            response.body() = contents.value();
            response.version(request.version());
            response.result(http::status::ok);
            response.set(http::field::content_type, "application/json");
            response.prepare_payload();

            return true;
        }
    }

    NotFoundHandler not_found_handler;
    return not_found_handler.handle_request(request, response);
}

status CRUDRequestHandler::handle_create(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response) {
    std::string request_str(request.target().data(), request.target().size());
    std::string directory_path = data_path_ + request_str.substr(path_.size());

    std::string body = "";
    status return_status = false;
    std::string content_type = "text/plain";
    http::status response_code = http::status::bad_request;

    // Check if directory exists and create a directory for the first entity:
    if (file_system_->initialize_directory(directory_path)) {
        std::string file_path = directory_path + "/1";

        Logger::log_trace("Created new entity directory: " + directory_path);
        Logger::log_trace("Creating new entity at: " + file_path);
        file_system_->write_file(file_path, request.body());

        response_code = http::status::created;
        content_type = "application/json";
        body = "{\"id\": 1}";
        return_status = true;
    } else {
        auto files = file_system_->list_files(directory_path);
        if (files) {
            int num_instances = (files.value()).size();
            std::vector<bool> ids(num_instances, false);
            // Find maximal ID suffix:
            for (auto path : files.value()) {
                // Assumption: Every file name is the ID of the entity:
                std::string suffix = get_suffix(path);
                int id = atoi(suffix.c_str());
                if (id - 1 < ids.size()) ids[id - 1] = true;
            }

            int next_id = num_instances + 1;

            for (int i = 0; i < ids.size(); i++) {
                if (!ids[i]) {
                    next_id = i;
                    break;
                }
            }

            next_id = std::min(next_id + 1, num_instances + 1);

            std::string new_id = std::to_string(next_id);
            std::string file_path = directory_path + "/" + new_id;

            Logger::log_trace("Creating new entity at: " + file_path);
            file_system_->write_file(file_path, request.body());

            response_code = http::status::created;
            content_type = "application/json";
            body = "{\"id\": " + new_id + "}";
            return_status = true;
        }
    }

    response.version(request.version());
    response.result(response_code);
    response.set(boost::beast::http::field::content_type, content_type);
    response.body() = body;
    response.prepare_payload();

    return return_status;
}

status CRUDRequestHandler::handle_delete(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response) {
    std::string request_str(request.target().data(), request.target().size());
    std::string file_path = data_path_ + request_str.substr(path_.size());

    http::status response_code = http::status::not_found;
    status return_status = false;

    // https://stackoverflow.com/questions/2342579/http-status-code-for-update-and-delete
    if (file_system_->delete_file(file_path)) {
        response_code = http::status::no_content;
        return_status = true;
    }

    response.version(request.version());
    response.result(response_code);
    response.set(boost::beast::http::field::content_type, "text/plain");
    response.body() = "";
    response.prepare_payload();
    return return_status;
}

status CRUDRequestHandler::handle_put(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response) {
    std::string request_str(request.target().data(), request.target().size());
    std::string file_path = data_path_ + request_str.substr(path_.size());
    std::string id_str = get_suffix(file_path);
    std::string directory_path =
        file_path.substr(0, file_path.size() - id_str.size() - 1);

    // Check if directory exists and create a directory for the first entity:
    if (file_system_->initialize_directory(directory_path)) {
        Logger::log_trace("Created new entity directory: " + directory_path);
        Logger::log_trace("Creating new entity at: " + file_path);
    } else {
        Logger::log_trace("Updating entity at: " + file_path);
    }

    file_system_->write_file(file_path, request.body());

    response.version(request.version());
    response.result(http::status::created);
    response.set(boost::beast::http::field::content_type, "text/plain");
    response.body() = "";
    response.prepare_payload();

    return true;
}

status CRUDRequestHandler::handle_request(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response) {
    Logger::log_trace("CRUD request");

    switch (request.method()) {
        case http::verb::get:
            Logger::log_trace("GET");
            return handle_get(request, response);
        case http::verb::post:
            Logger::log_trace("POST");
            return handle_create(request, response);
        case http::verb::delete_:
            Logger::log_trace("DELETE");
            return handle_delete(request, response);
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
