#include "http_request.h"

#include <cctype>
#include <regex>
#include <sstream>
#include <unordered_set>

bool validHeaderName(const std::string& headerName) {
    int n = headerName.size();
    if (n == 0) {
        return false;
    }
    if (!isalpha(headerName[0]) || !isalpha(headerName[n - 1])) {
        return false;
    }
    bool lastIsHyphen = false;
    for (auto c : headerName) {
        if (lastIsHyphen && c == '-') {
            return false;
        }
        lastIsHyphen = (c == '-');
        if (!isalpha(c) && c != '-') {
            return false;
        }
    }
    return true;
}

bool http_request::parseRequest(http_request& req, const std::string& req_str) {
    std::istringstream req_stream(req_str);

    req.raw = req_str;

    std::string line;
    if (!std::getline(req_stream, line)) {
        return false;
    }

    // Parse method, URI, and http method from first line:
    std::istringstream first_line(line);
    std::string http_version_str;
    if (!(first_line >> req.method >> req.uri >> http_version_str)) {
        return false;
    }

    while (!req.uri.empty() && req.uri[0] == '/') {
        req.uri = req.uri.substr(1);
    }
    while (!req.uri.empty() && req.uri.back() == '/') {
        req.uri.pop_back();
    }

    // Ensure the method in the header is valid:
    if (req.method != "GET") {
        return false;
    }

    if (std::sscanf(http_version_str.c_str(), "HTTP/%d.%d",
                    &req.http_version_major, &req.http_version_minor) != 2) {
        return false;
    }

    // Parse the headers (read the rest of the request string):
    while (std::getline(req_stream, line)) {
        std::string name, value;
        std::istringstream header_line(line);
        if (std::getline(header_line, name, ':') &&
            std::getline(header_line, value)) {
            if (!validHeaderName(name)) {
                return false;
            }
            // Trim value:
            value = std::regex_replace(value, std::regex("^ +| +$"), "$1");
            req.headers.push_back({name, value});
        } else {
            return false;
        }
    }

    return true;
}
