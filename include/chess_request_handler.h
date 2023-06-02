#ifndef CHESS_REQUEST_HANDLER_H
#define CHESS_REQUEST_HANDLER_H

#include <memory>
#include <string>
#include <vector>

#include "config_parser.h"
#include "file_system.h"
#include "request_handler.h"

namespace http = boost::beast::http;

class ChessRequestHandler : public RequestHandler {
    public:
        ChessRequestHandler(const std::string& path,
                            const std::string& data_path,
                            std::shared_ptr<FileSystem> file_system);

        status handle_request(const http::request<http::string_body>& request,
                              http::response<http::string_body>& response);

    private:
        std::vector<std::string> parse_request_path(std::string request_str);

        status handle_get(const http::request<http::string_body>& request,
                          http::response<http::string_body>& response);
        status handle_create(const http::request<http::string_body>& request,
                             http::response<http::string_body>& response);
        status handle_put(const http::request<http::string_body>& request,
                          http::response<http::string_body>& response);

        std::string path_;
        std::string data_path_;
        std::shared_ptr<FileSystem> file_system_;
};

#endif