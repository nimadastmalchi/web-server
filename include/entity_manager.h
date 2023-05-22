#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <map>
#include <string>
#include <vector>
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <boost/log/trivial.hpp>

#include "not_found_handler.h"

class EntityManager {
    public:
        EntityManager();
        bool insert(const std::string& path, const std::string& root,
                                const boost::beast::http::request<boost::beast::http::string_body>& request,
                                boost::beast::http::response<boost::beast::http::string_body>& response);
        bool get_entity(const std::string& path, const std::string& root,
                                const boost::beast::http::request<boost::beast::http::string_body>& request,
                                boost::beast::http::response<boost::beast::http::string_body>& response);

    private:
        std::map<std::string, int> map_type_to_id{};
        std::vector<std::string> split_request(const std::string& path, const std::string& root,const std::string& request_target);
};

#endif