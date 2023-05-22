#ifndef REQUEST_HANDLER_CRUD_H
#define REQUEST_HANDLER_CRUD_H

#include <string>

#include "request_handler.h"
#include "entity_manager.h"

class CRUDRequestHandler : public RequestHandler {
    public:
        CRUDRequestHandler(const std::string& path, const std::string& root, EntityManager& entity_manager);
        
        status handle_request(
            const boost::beast::http::request<boost::beast::http::string_body>&
                request,
            boost::beast::http::response<boost::beast::http::string_body>&
                response) override;
    private:
        status handle_create(
            const boost::beast::http::request<boost::beast::http::string_body>& request,
            boost::beast::http::response<boost::beast::http::string_body>& response);
        status handle_retrieve_or_list(
            const boost::beast::http::request<boost::beast::http::string_body>& request,
            boost::beast::http::response<boost::beast::http::string_body>& response);
        status handle_update(
            const boost::beast::http::request<boost::beast::http::string_body>& request,
            boost::beast::http::response<boost::beast::http::string_body>& response);
        status handle_delete(
            const boost::beast::http::request<boost::beast::http::string_body>& request,
            boost::beast::http::response<boost::beast::http::string_body>& response);
        std::string root_;
        std::string prefix_;
        EntityManager& entity_manager_;

};

#endif