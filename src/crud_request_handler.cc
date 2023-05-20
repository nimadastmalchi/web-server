#include "crud_request_handler.h"

using namespace boost::beast;

CRUDRequestHandler::CRUDRequestHandler(const std::string& path,
                                           const std::string& root)
    : prefix_(path), root_(root) {
}

status CRUDRequestHandler::handle_request(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response)
{
    switch(request.method()){
        case http::verb::post:
            return handle_create(request,response);
        case http::verb::get: {
            std::string target_string(request.target().data(), request.target().size());
            target_string = target_string.substr(prefix_.size());
            
            int count = 0;
            
            for(size_t i=0;i<target_string.size();i++)
            {
                if(target_string[i] == '/')
                {
                    count+=1;
                }
            }
            if(count == 1)
            {
                return handle_list(request,response);
            }
            else if(count == 2)
            {
                return handle_retrieve(request,response);
            }
            else
            {
                // Return bad request
                response.version(request.version());
                response.result(400);
                response.set(http::field::content_type, "text/plain");
                response.body() = "";
                response.prepare_payload();
            }
        }
        case http::verb::put:
            return handle_update(request,response);
        case http::verb::delete_:
            return handle_delete(request,response);
        default:
            // Return method not allowed
            response.version(request.version());
            response.result(http::status::method_not_allowed);
            response.set(http::field::content_type, "text/plain");
            response.body() = "";
            response.prepare_payload();
    }
    return true;
}

status CRUDRequestHandler::handle_create(
    const boost::beast::http::request<boost::beast::http::string_body>& request,
    boost::beast::http::response<boost::beast::http::string_body>& response)
{
    // To be implemented
    return true;
}


status CRUDRequestHandler::handle_retrieve(
    const boost::beast::http::request<boost::beast::http::string_body>& request,
    boost::beast::http::response<boost::beast::http::string_body>& response)
{
    // To be implemented
    return true;
}

status CRUDRequestHandler::handle_update(
    const boost::beast::http::request<boost::beast::http::string_body>& request,
    boost::beast::http::response<boost::beast::http::string_body>& response)
{
    // To be implemented
    return true;
}

status CRUDRequestHandler::handle_delete(
    const boost::beast::http::request<boost::beast::http::string_body>& request,
    boost::beast::http::response<boost::beast::http::string_body>& response)
{
    // To be implemented
    return true;
}

status CRUDRequestHandler::handle_list(
    const boost::beast::http::request<boost::beast::http::string_body>& request,
    boost::beast::http::response<boost::beast::http::string_body>& response)
{
    // To be implemented
    return true;
}