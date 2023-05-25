#include "entity_manager.h"
#include <iostream>

EntityManager::EntityManager(FileSystem* fs)
{
    this->fs = fs;
}

// split_request takes a request "api/shoes/1" and returns tokenized array [shoes,1]
// it accounts for nested path from location
std::vector<std::string> EntityManager::split_request(const std::string& path,const std::string& request_target)
{
    std::string shortened_request = request_target.substr(path.size());
    std::vector<std::string> split;
    if(shortened_request.size() == 0)
    {
        return split;
    }
    
    std::string temp_string;
    for(size_t i = 1; i <shortened_request.length(); i++){
        if(shortened_request[i]=='/'){
            split.push_back(temp_string);
            temp_string = "";
        }
        else{
           temp_string += shortened_request[i];
        }
    }
    split.push_back(temp_string);

    return split;
}

// sets response for new entity related status codes
void EntityManager::set_response(int status_code,const boost::beast::http::request<boost::beast::http::string_body>& request,boost::beast::http::response<boost::beast::http::string_body>& response,
                                 std::string optional_file_content)
{
    // if status code is 404 notFoundHandler would set these values twice
    if(status_code != 404)
    {
        response.version(request.version());
        response.result(status_code);
    }

    // accounting for all possible status codes in entity manager
    if(status_code == 400)
    {
        response.set(boost::beast::http::field::content_type, "text/html");
        response.body() = "<html><head><title>400 Bad Request</title></head><body><p>400 Bad Request</p></body></html>";
    }
    else if(status_code == 201)
    {
        response.set(boost::beast::http::field::content_type, "application/json");
        response.body() = optional_file_content;
    }
    else if(status_code == 404)
    {
        NotFoundHandler not_found_handler;
        not_found_handler.handle_request(request, response);
    }
    else if(status_code == 200)
    {
        response.set(boost::beast::http::field::content_type, "application/json");
        response.body() = optional_file_content;
    }
    else // status code 204
    {
        response.body() = "";
    }
    
    response.prepare_payload();

}

bool EntityManager::insert(const std::string& path, const std::string& root,
                                      const boost::beast::http::request<boost::beast::http::string_body>& request,
                                      boost::beast::http::response<boost::beast::http::string_body>& response,
                                      int optional_insert_id)
{
    std::string request_target(request.target().data(), request.target().size());
    std::vector<std::string> split = split_request(path,request_target);
    
    // These conditions would signify invalid parameters if optional_insert_id not passed
    if((optional_insert_id == -1) && (split.size() >= 2 || split.size() == 0))
    {
        set_response(400,request,response);
        return true;
    }
    
    std::string directory_path = root + "/" + split[0];
    std::string body = "{EXAMPLE PAYLOAD: this codebase does not store request body into boost::beast request}";
    std::string list_path = directory_path + "/list";

    std::string file_path;
    
    int id_insert;
    if(optional_insert_id != 1)
    {
        file_path = directory_path + "/" + std::to_string(optional_insert_id);
        id_insert = optional_insert_id;        
    }

    // Check if directory already exists in system
    if (fs->exists_directory(directory_path)) {
        
        if(optional_insert_id == -1)
        {
            // Check if file_path is already occupied at point we are inserting
            int attempt_insert_id = 1;
            std::string attempt_insert_file_path = directory_path + "/" + std::to_string(attempt_insert_id);
            while(fs->exists_file(attempt_insert_file_path))
            {
                attempt_insert_id += 1;
                attempt_insert_file_path = directory_path + "/" + std::to_string(attempt_insert_id);
            }
            // Once the while loop is broken we know the place of insertion is not occupied
            file_path = attempt_insert_file_path;
            id_insert = attempt_insert_id;
        }
        std::string list_contents = fs->read_file(list_path);
        if (list_contents.size() != 0) {
            list_contents = list_contents.substr(0, list_contents.size() - 1) + "," + std::to_string(id_insert) + "]";
        }
        fs->write_file(list_path, list_contents);

        std::string body;
        if(optional_insert_id == -1)
        {
            body = "{EXAMPLE PAYLOAD: this codebase does not store request body into boost::beast request}";
        }
        else
        {
            body = "{EXAMPLE UPDATED PAYLOAD: this codebase does not store request body into boost::beast request}";
        }
        fs->write_file(list_path, list_contents);
        fs->create_file(file_path);
        fs->write_file(file_path, body);
    }
    else
    {
        if (fs->create_directory(directory_path))
        {
            if(optional_insert_id == -1)
            {
                id_insert = 1;
                file_path = directory_path + "/1";
            }

            //create empty list that will track ids
            fs->create_file(list_path);
            fs->write_file(list_path, "[" + std::to_string(id_insert) + "]");

            std::string body;
            if(optional_insert_id == -1)
            {
                body = "{EXAMPLE PAYLOAD: this codebase does not store request body into boost::beast request}";
            }
            else
            {
                body = "{EXAMPLE UPDATED PAYLOAD: this codebase does not store request body into boost::beast request}";
            }

            fs->create_file(file_path);
            fs->write_file(file_path, body);
        }
        else
        {
            Logger::log_info("Directory failed to create");
            return false;
        }
    }
    
    //set response
    std::string content = "{\"id\": "+std::to_string(id_insert)+"}";
    set_response(201,request,response,content);
    return true;
}

bool EntityManager::get_entity(const std::string& path, const std::string& root,
                                const boost::beast::http::request<boost::beast::http::string_body>& request,
                                boost::beast::http::response<boost::beast::http::string_body>& response)
{
    std::string request_target(request.target().data(), request.target().size());
    std::vector<std::string> split= split_request(path,request_target);
    std::string directory_path;

    if(split.size() == 1)
    {
        std::string entity_name = split[0];
        directory_path = root + "/" + entity_name + "/" + "list";
    }
    else if (split.size() == 2)
    {
        std::string entity_name = split[0];
        std::string entity_number = split[1];
        directory_path = root + "/" + entity_name + "/" + entity_number;      
    }
    else
    {
        set_response(400,request,response);
        return true;
    }

    if(fs->exists_file(directory_path))
    {   
        std::string content = fs->read_file(directory_path);
        set_response(200,request,response,content);
    }
    else
    {
        set_response(404,request,response);
    }
    return true;
}


bool EntityManager::update(const std::string& path, const std::string& root,
                                const boost::beast::http::request<boost::beast::http::string_body>& request,
                                boost::beast::http::response<boost::beast::http::string_body>& response)
{
    std::string request_target(request.target().data(), request.target().size());
    std::vector<std::string> split= split_request(path,request_target);

    if(split.size() == 2)
    {
        std::string entity_name = split[0];
        std::string entity_number = split[1];
        std::string file_path = root + "/" + entity_name + "/" + entity_number; 

        //Check existance of file_path
        if(fs->exists_file(file_path))
        {
            if (!fs->delete_file(file_path)) {
                Logger::log_info("Filesystem error occurred while removing the file");
                return false;
            }

            //update payload of new file
            std::string body = "{EXAMPLE UPDATED PAYLOAD: this codebase does not store request body into boost::beast request}";
            fs->create_file(file_path);
            fs->write_file(file_path, body);

            std::string content = "{\"id\": "+entity_number+"}";
            set_response(200,request,response,content);
        }
        else // if file doesn't exist insert new entity
        {
            insert(path,root,request,response,std::stoi(entity_number));
        }
        
        return true;
    }
    else // split_size != 2 shows bad request
    {
        set_response(400,request,response);
    }

    return true;
}

bool EntityManager::delete_(const std::string& path, const std::string& root,
                                const boost::beast::http::request<boost::beast::http::string_body>& request,
                                boost::beast::http::response<boost::beast::http::string_body>& response)
{
    std::string request_target(request.target().data(), request.target().size());
    std::vector<std::string> split = split_request(path,request_target);

    if(split.size() == 2)
    {
        std::string entity_name = split[0];
        std::string entity_number = split[1];
        std::string directory_path = root + "/" + entity_name;
        std::string list_path = directory_path + "/list";
        std::string file_path = root + "/" + entity_name + "/" + entity_number; 

        if(fs->exists_file(file_path))
        {
            if (!fs->delete_file(file_path)) {
                Logger::log_info("Error occurred while initially removing the file in EntityManager::delete_");
                return false;
            }
            std::string file_contents = fs->read_file(list_path);

            std::string match_middle("," + entity_number + ",");
            std::string match_front("[" + entity_number + ",");
            std::string match_back("," + entity_number + "]");

            size_t pos;
            if ((pos = file_contents.find(match_middle)) != std::string::npos) 
            {
                file_contents.replace(pos, match_middle.length(), ",");
                fs->write_file(list_path, file_contents);
            }
            else if ((pos = file_contents.find(match_front)) != std::string::npos) 
            {
                file_contents.replace(pos, match_front.length(), "[");
                fs->write_file(list_path, file_contents);
            }
            else if ((pos = file_contents.find(match_back)) != std::string::npos) 
            {
                file_contents.replace(pos, match_back.length(), "]");
                fs->write_file(list_path, file_contents);
            }
            else {
                //first delete list
                if (!fs->delete_file(list_path)) 
                {
                    Logger::log_info("Error occurred while removing the list file");
                    return false;
                }
                //then delete directory
                if (!fs->delete_directory(directory_path)) 
                {
                    Logger::log_info("Error occurred while removing the entity directory");
                    return false;
                }   
            }
            
            // set response
            set_response(204,request,response);
        }
        else // if path doesn't exist we return not found
        {
            set_response(404,request,response);
        }
    }
    else // split_size != 2 shows bad request
    {
        set_response(400,request,response);
    }
    return true;
}