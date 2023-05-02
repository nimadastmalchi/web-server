#include <string>
#include <vector>

class http_header {
    public:
        std::string name;
        std::string value;
};

class http_request {
    public:
        std::string method;
        std::string uri;
        int http_version_major;
        int http_version_minor;
        std::vector<http_header> headers;

        // Given a raw HTTP request string, req_str, populate the http_request
        // struct, req.
        // Returns true if req_str is valid and req is successfully populated.
        // Returns false otherwise. In this case, req may be partially
        // populated.
        static bool parseRequest(http_request& req, const std::string& req_str);
};
