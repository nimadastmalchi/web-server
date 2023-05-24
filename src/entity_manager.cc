#include "entity_manager.h"

EntityManager::EntityManager()
{
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

    std::vector<std::string> split= split_request(path,request_target);
    
    // These conditions would signify invalid parameters if optional_insert_id not passed
    if((optional_insert_id == -1) && (split.size() >= 2 || split.size() == 0))
    {
        set_response(400,request,response);
        return true;
    }
    
    std::string entity_name = split[0];

    std::string directory_path = root + "/" + entity_name;

    std::string file_path;
    
    int id_insert;

    if(optional_insert_id != 1)
    {
        file_path = directory_path + "/" + std::to_string(optional_insert_id);
        id_insert = optional_insert_id;        
    }

    // Check if directory already exists in system
    if (std::filesystem::exists(directory_path) && std::filesystem::is_directory(directory_path)) {
        
        if(optional_insert_id == -1)
        {
            // Check if file_path is already occupied at point we are inserting
            int attempt_insert_id = 1;
            std::string attempt_insert_file_path = directory_path + "/" + std::to_string(attempt_insert_id);
            while(std::filesystem::exists(attempt_insert_file_path))
            {
                attempt_insert_id += 1;
                attempt_insert_file_path = directory_path + "/" + std::to_string(attempt_insert_id);
            }
            // Once the while loop is broken we know the place of insertion is not occupied
            file_path = attempt_insert_file_path;
            id_insert = attempt_insert_id;
        }
        
        std::ifstream file_in(directory_path+"/list");
        std::stringstream buffer;
        buffer << file_in.rdbuf();
        std::string file_contents = buffer.str();
        file_in.close();

        // Modify the contents
        if (!file_contents.empty()) {
            // Remove the closing bracket ("]") from the current contents
            file_contents.pop_back();

            // Add the new value to the contents
            file_contents += ","+std::to_string(id_insert) +"]";
        }

        // Write the modified contents back to the file
        std::ofstream file_out(directory_path+"/list");
        file_out << file_contents;
        file_out.close();

        // Note: the implementation in this codebase does not put the body of the request into boost::beast request
        // so we will use a temporary implementation
        // const boost::beast::string_view body = request.body(); -> doesn't return anything
        std::string body;
        if(optional_insert_id == -1)
        {
            body = "{EXAMPLE PAYLOAD: this codebase does not store request body into boost::beast request}";
        }
        else
        {
            body = "{EXAMPLE UPDATED PAYLOAD: this codebase does not store request body into boost::beast request}";
        }

        std::ofstream file_in_2(file_path);
        file_in_2 << body;
        file_in_2.close();
    }
    else
    {
        // create and initialize directory
        if (std::filesystem::create_directory(directory_path)){

            if(optional_insert_id == -1)
            {
                id_insert = 1;
                file_path = directory_path + "/1";
            }

            //create empty list that will track ids
            std::ofstream file_in(directory_path + "/list");
            file_in << "[" + std::to_string(id_insert) + "]";
            file_in.close();

            // Note: the implementation in this codebase does not put the body of the request into boost::beast request
            // so we will use a temporary implementation
            // const boost::beast::string_view body = request.body(); -> doesn't return anything
            std::string body;
            if(optional_insert_id == -1)
            {
                body = "{EXAMPLE PAYLOAD: this codebase does not store request body into boost::beast request}";
            }
            else
            {
                body = "{EXAMPLE UPDATED PAYLOAD: this codebase does not store request body into boost::beast request}";
            }

            std::ofstream file_in_2(file_path);
            file_in_2 << body;
            file_in_2.close();
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
        // Return bad request
        set_response(400,request,response);
        return true;
    }

    
    if(std::filesystem::exists(directory_path))
    {   
        std::ifstream file(directory_path);
        if (!file.is_open())
        {
            Logger::log_info("File failed to open");
            return false;
        }

        std::stringstream req_stream;
        req_stream << file.rdbuf(); // Read file contents into req_stream
        file.close();

        std::string content = req_stream.str();
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
        if(std::filesystem::exists(file_path))
        {
            try {
                std::filesystem::remove(file_path);
            } catch (const std::filesystem::filesystem_error& error) {
                Logger::log_info("Error occurred while removing the file.");
                return false;
            } catch (...) {
                Logger::log_info("Unknown error occurred while removing the file.");
                return false;
            }

            //update payload of new file
            std::string body = "{EXAMPLE UPDATED PAYLOAD: this codebase does not store request body into boost::beast request}";

            std::ofstream file_in(file_path);
            file_in << body;
            file_in.close();

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
    // Delete individual file
    std::string request_target(request.target().data(), request.target().size());

    std::vector<std::string> split= split_request(path,request_target);

    if(split.size() == 2)
    {
        std::string entity_name = split[0];

        std::string entity_number = split[1];

        std::string directory_path = root + "/" + entity_name;
        std::string file_path = root + "/" + entity_name + "/" + entity_number; 

        if(std::filesystem::exists(file_path))
        {
            //delete file
            try {
            std::filesystem::remove(file_path);
            } catch (const std::filesystem::filesystem_error& error) {
                Logger::log_info("Error occurred while removing the file.");
                return false;
            } catch (...) {
                Logger::log_info("Unknown error occurred while removing the file.");
                return false;
            }

            //Read stream of list file
            std::ifstream file_in_2(directory_path+"/list");
            std::stringstream buffer;
            buffer << file_in_2.rdbuf();
            std::string file_contents = buffer.str();
            file_in_2.close();

            std::string match_middle("," + entity_number + ",");
            std::string match_front("[" + entity_number + ",");
            std::string match_back("," + entity_number + "]");

            size_t pos;
            if ((pos = file_contents.find(match_middle)) != std::string::npos) {
                file_contents.replace(pos, match_middle.length(), ",");
                std::ofstream file_out(directory_path+"/list");
                file_out << file_contents;
                file_out.close();
            }
            else if ((pos = file_contents.find(match_front)) != std::string::npos) {
                file_contents.replace(pos, match_front.length(), "[");
                std::ofstream file_out(directory_path+"/list");
                file_out << file_contents;
                file_out.close();
            }
            else if ((pos = file_contents.find(match_back)) != std::string::npos) {
                file_contents.replace(pos, match_back.length(), "]");
                std::ofstream file_out(directory_path+"/list");
                file_out << file_contents;
                file_out.close();
            }
            else {
                // first delete list
                try {
                std::filesystem::remove(directory_path+"/list");
                } catch (const std::filesystem::filesystem_error& error) {
                    Logger::log_info("Error occurred while removing the file.");
                    return false;
                } catch (...) {
                    Logger::log_info("Unknown error occurred while removing the file.");
                    return false;
                }

                // then delete directory
                try {
                std::filesystem::remove(directory_path);
                } catch (const std::filesystem::filesystem_error& error) {
                    Logger::log_info("Error occurred while removing the file.");
                    return false;
                } catch (...) {
                    Logger::log_info("Unknown error occurred while removing the file.");
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