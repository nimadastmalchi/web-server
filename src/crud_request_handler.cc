#include "crud_request_handler.h"

using namespace boost::beast;

CRUDRequestHandler::CRUDRequestHandler(const std::string& path,
                                       const std::string& root,
                                      EntityManager& entity_manager)
    : prefix_(path), root_(root), entity_manager_(entity_manager) {
}

status CRUDRequestHandler::handle_request(
    const http::request<http::string_body>& request,
    http::response<http::string_body>& response)
{
    switch(request.method()){
        case http::verb::post:
            return handle_create(request,response);
        case http::verb::get: {
            return handle_retrieve_or_list(request,response);
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
            return false;
    }
}

status CRUDRequestHandler::handle_create(
    const boost::beast::http::request<boost::beast::http::string_body>& request,
    boost::beast::http::response<boost::beast::http::string_body>& response)
{
    return entity_manager_.insert(prefix_,root_,request,response);
}


status CRUDRequestHandler::handle_retrieve_or_list(
    const boost::beast::http::request<boost::beast::http::string_body>& request,
    boost::beast::http::response<boost::beast::http::string_body>& response)
{
    return entity_manager_.get_entity(prefix_,root_,request,response);
}

status CRUDRequestHandler::handle_update(
    const boost::beast::http::request<boost::beast::http::string_body>& request,
    boost::beast::http::response<boost::beast::http::string_body>& response)
{
    return entity_manager_.update(prefix_,root_,request,response);
}

status CRUDRequestHandler::handle_delete(
    const boost::beast::http::request<boost::beast::http::string_body>& request,
    boost::beast::http::response<boost::beast::http::string_body>& response)
{
    return entity_manager_.delete_(prefix_,root_,request,response);
}
