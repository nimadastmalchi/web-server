#include "entity_manager.h"

EntityManager::EntityManager()
{
}

// split_request takes a request "api/shoes/1" and returns tokenized array [shoes,1]
// it accounts for nested path from location
std::vector<std::string> EntityManager::split_request(const std::string& path, const std::string& root,const std::string& request_target)
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

bool EntityManager::insert(const std::string& path, const std::string& root,
                                      const boost::beast::http::request<boost::beast::http::string_body>& request,
                                      boost::beast::http::response<boost::beast::http::string_body>& response)
{
    std::string request_target(request.target().data(), request.target().size());

    std::vector<std::string> split= split_request(path,root,request_target);
    
    // These conditions would signify invalid parameters
    if(split.size() >= 2 || split.size() == 0)
    {
        response.version(request.version());
        response.result(400);
        response.set(boost::beast::http::field::content_type, "text/plain");
        response.body() = "";
        response.prepare_payload();
        return true;
    }
    
    std::string entity_name = split[0];

    std::string directory_path = root + "/" + entity_name;

    // Check if directory already exists in system
    if (std::filesystem::exists(directory_path) && std::filesystem::is_directory(directory_path)) {
        map_type_to_id[entity_name] += 1;
        
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
            file_contents += ","+std::to_string(map_type_to_id[entity_name]) +"]";
        }

        // Write the modified contents back to the file
        std::ofstream file_out(directory_path+"/list");
        file_out << file_contents;
        file_out.close();

        // Note: the implementation in this codebase does not put the body of the request into boost::beast request
        // so we will use a temporary implementation
        // const boost::beast::string_view body = request.body(); -> doesn't return anything
        std::string body = "{EXAMPLE PAYLOAD: this codebase does not store request body into boost::beast request}";

        std::ofstream file_in_2(directory_path +"/"+ std::to_string(map_type_to_id[entity_name]));
        file_in_2 << body;
        file_in_2.close();
    }
    else
    {
        // create and initialize directory
        if (std::filesystem::create_directory(directory_path)){
            
            //create empty list that will track ids
            std::ofstream file_in(directory_path + "/list");
            file_in << "[1]";
            file_in.close();

            //initialize map that tracks count of ids
            map_type_to_id[entity_name] = 1;


            // Note: the implementation in this codebase does not put the body of the request into boost::beast request
            // so we will use a temporary implementation
            // const boost::beast::string_view body = request.body(); -> doesn't return anything
            std::string body = "{EXAMPLE PAYLOAD: this codebase does not store request body into boost::beast request}";

            std::ofstream file_in_2(directory_path +"/"+ std::to_string(map_type_to_id[entity_name]));
            file_in_2 << body;
            file_in_2.close();
        }
        else
        {
            BOOST_LOG_TRIVIAL(info) << "Directory failed to create";
            return false;
        }
    }
    
    //set response
    std::string content = "{\"id\": "+std::to_string(map_type_to_id[entity_name])+"}";
    response.version(request.version());
    response.result(200);
    response.set(boost::beast::http::field::content_type, "application/json");
    response.body() = content;
    response.prepare_payload();
    return true;
}

bool EntityManager::get_entity(const std::string& path, const std::string& root,
                                const boost::beast::http::request<boost::beast::http::string_body>& request,
                                boost::beast::http::response<boost::beast::http::string_body>& response)
{
    std::string request_target(request.target().data(), request.target().size());

    std::vector<std::string> split= split_request(path,root,request_target);

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
        response.version(request.version());
        response.result(400);
        response.set(boost::beast::http::field::content_type, "text/plain");
        response.body() = "";
        response.prepare_payload();
        return true;
    }

    
    if(std::filesystem::exists(directory_path))
    {   
        std::ifstream file(directory_path);
        if (!file.is_open())
        {
            BOOST_LOG_TRIVIAL(info)<<"File failed to open";
            return false;
        }

        std::stringstream req_stream;
        req_stream << file.rdbuf(); // Read file contents into req_stream
        file.close();

        std::string content = req_stream.str();
        response.version(request.version());
        response.result(200);
        response.set(boost::beast::http::field::content_type, "application/json");
        response.body() = content;
        response.prepare_payload();
    }
    else
    {
        NotFoundHandler not_found_handler;
        not_found_handler.handle_request(request, response);
    }
    return true;
}