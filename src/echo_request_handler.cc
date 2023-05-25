#include "echo_request_handler.h"

#include <sstream>
#include <string>

using namespace boost::beast;

status EchoRequestHandler::handle_request(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response) {
    std::stringstream req_stream;
    req_stream << request;
    std::string content = req_stream.str();
    response.version(request.version());
    response.result(http::status::ok);
    response.set(http::field::content_type, "text/plain");
    response.body() = content;
    response.prepare_payload();
    return true;
}
