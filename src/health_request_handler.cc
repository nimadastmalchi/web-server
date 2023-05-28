#include "health_request_handler.h"

#include <sstream>
#include <string>

using namespace boost::beast;

status HealthRequestHandler::handle_request(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response) {
    response.version(request.version());
    response.result(http::status::ok);
    response.set(http::field::content_type, "text/plain");
    response.body() = "OK";
    response.prepare_payload();
    return true;
}
