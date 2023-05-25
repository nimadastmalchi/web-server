#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <string>
#include <vector>
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "file_system.h"
#include "not_found_handler.h"
#include "logger.h"

class EntityManager {
    public:
        EntityManager(FileSystem* fs);
        bool insert(const std::string& path, const std::string& root,
                                const boost::beast::http::request<boost::beast::http::string_body>& request,
                                boost::beast::http::response<boost::beast::http::string_body>& response,
                                int optional_insert_id = -1);
        bool get_entity(const std::string& path, const std::string& root,
                                const boost::beast::http::request<boost::beast::http::string_body>& request,
                                boost::beast::http::response<boost::beast::http::string_body>& response);

        bool update(const std::string& path, const std::string& root,
                                const boost::beast::http::request<boost::beast::http::string_body>& request,
                                boost::beast::http::response<boost::beast::http::string_body>& response);

        bool delete_(const std::string& path, const std::string& root,
                                const boost::beast::http::request<boost::beast::http::string_body>& request,
                                boost::beast::http::response<boost::beast::http::string_body>& response);
        
    private:
        FileSystem* fs;
        std::vector<std::string> split_request(const std::string& path,const std::string& request_target);
        void set_response(int status_code, const boost::beast::http::request<boost::beast::http::string_body>& request, boost::beast::http::response<boost::beast::http::string_body>& response,
                                std::string optional_file_content = "");
};

#endif