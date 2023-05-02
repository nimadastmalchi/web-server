#ifndef RESPONSE_BUILDER_H
#define RESPONSE_BUILDER_H

#include <iostream>

class ResponseBuilder {
    public:
        ResponseBuilder();
        int formatResponse(char data[], const std::string& delimiter,
                           const std::string& response_code,
                           const std::string& content_type);
        std::string getResponse();
        void resetResponse();

    private:
        std::string response_;
};

#endif