#include "sleep_request_handler.h"

#include <unistd.h>

#include <iostream>
#include <string>

#include "config_parser.h"
#include "not_found_handler.h"
#include "request_handler.h"

using namespace boost::beast;

SleepRequestHandler::SleepRequestHandler(const std::string& path,
                                         const int sleep_seconds)
    : prefix_(path), sleep_seconds_(sleep_seconds) {}

// Sets HTTP response string according to parsed parameters from input request.
status SleepRequestHandler::handle_request(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response) {
    sleep(sleep_seconds_);

    response.result(http::status::ok);
    response.version(request.version());
    response.prepare_payload();

    return true;
}
