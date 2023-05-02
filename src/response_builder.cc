#include "response_builder.h"

#include <cstring>
#include <iostream>

ResponseBuilder::ResponseBuilder() : response_("") {}

// Exit codes:
// 0: Delimiter not yet reached.
// 1: Response constructed.
int ResponseBuilder::formatResponse(char data[], const std::string& delimiter,
                                    const std::string& response_code,
                                    const std::string& content_type) {
    // Check whether the delimiter exists:
    char* header_end = strstr(data, delimiter.c_str());

    // If it does exist, we have received the entire header:
    if (header_end) {
        int header_size = header_end - data;

        // Delimit the end of the header with NULL:
        data[header_size] = 0;
        std::string content = data;

        // Construct the response:
        response_ = response_code + content_type + content + "\r\n";

        return 1;
    }

    return 0;
}

std::string ResponseBuilder::getResponse() { return response_; }

void ResponseBuilder::resetResponse() { response_ = ""; }
