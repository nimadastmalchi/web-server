# a2-team-please-adopt-me-nima

## Source Code Layout
### Directory structure
The web server is written in C++ and uses the Boost library. 

Class files are split into header (`.h`) and implementation (`.cc`) files. All header files are in the `include` directory, whereas implementation files are in `src`.

Every class in our codebase has associated unit tests, as well as overall end to end tests. All testing source code can be located in the `tests` directory. 

### System design
The entry point into our server executable is `src/server_main.cc`. The following design diagram outlines (in brief) the logical dependencies in our server loop.

![Diagram depicting server loop. Port and handler mappings are parsed from the config file, which are then passed to server to initialize sessions. A session listens for requests, parses requests, and then creates and dispactches the appropriate handler from a factory according to the handler mapping.](diagram.jpeg)


## Build, Test and Run
### Build
Per project protocol, all builds must be out-of-source. To build the project, run the following bash commands in the root directory.
```bash
$ mkdir build && cd build
$ cmake ..
$ make
```

### Test

Once the project has been built, you can run the test suite from the build directory via:
```bash
$ make test
```

To generate a test code coverage report, run the following bash commands from the root directory. 

```bash
$ mkdir coverage && cd coverage
$ cmake -DCMAKE_BUILD_TYPE=Coverage ..
$ make
$ make coverage
```

The coverage report can be found in `coverage/report/index.html`.

### Run

To run the server, run the following command from the root directory.
```bash
./build/bin/server ./src/server_config
```

The second argument is the config file, which you can modify to define custom server configuraton.



## How to Add a Request Handler
To demonstrate how to add a new Request Handler to our server, we walk through an example with the `StaticRequestHandler` class. Note that we've elected to omit other header/library include commands, as well as include guards, for clarity. 


### 1. Define the new request handler as an inherited class
All new request handlers should inherit from the `RequestHandler` interface defined in `include/request_handler.h`. Header files should be defined in the `include` directory, and class implementations should be defined in the `src` directory.

Header file: `include/static_request_handler.h`
```C++
// Include RequestHandler interface header
#include "request_handler.h"
...

// Publicly inherit from RequestHandler interface
class StaticRequestHandler : public RequestHandler {
    public:
        // Constructor defines data types required to serve requests 
        StaticRequestHandler(const std::string& path, const std::string& root);

        // Override the inherited handle_request virtual method
        status handle_request(const boost::beast::http::request<boost::beast::http::string_body>&
                request,
            boost::beast::http::response<boost::beast::http::string_body>&
                response) override;

    // Define helper functions and member variables privately
    private:
        ...
};
```
By inheriting from `RequestHandler`, any new request handler is required to implement the handle_request function, as well as its own constructor.

Implementation file: `src/static_request_handler.cc`
```C++
// Include header file to implement
#include "static_request_handler.h"
...

// Implement function headers from `static_request_handler.h`. Remember to define name space
StaticRequestHandler::StaticRequestHandler(const std::string& path, const std::string& root) {
    ...
}

// handle_request should populate response object according to request
status StaticRequestHandler::handle_request(const boost::beast::http::request<boost::beast::http::string_body>&
                request,
            boost::beast::http::response<boost::beast::http::string_body>&
                response) {
    ...
}

// Implement private function headers
...

```
Note that all implementations of `handle_request` should populate the input response object with fully formed responses. Dispatchers should be able to treat populated responses as valid HTTP responses.

New request handlers should also be fully configured at construction time. Therefore, all request serving customization should be parsed from the config file. 

### 2. Define a factory class for the new request handler
Per the class API, request handlers are dynamically created within a session using factory functions. To define a factory class for a new request handler, inherit from the factory interface defined in `include/request_handler_factory.h`.


Header file: `include/static_request_handler_factory.h`
```C++
// Include RequestHandlerFactory and RequestHandler interface headers
#include "request_handler_factory.h"
#include "request_handler.h"
...

// Publicly inherit from RequestHandlerFactory interface
class StaticRequestHandlerFactory : public RequestHandlerFactory {
    public:
        // Factory constructor is responsible for parsing config itself
        StaticRequestHandlerFactory(const std::string& path, const NginxConfig& config);
        
        // createHandler should take no inputs to conform with dispatcher
        std::shared_ptr<RequestHandler> createHandler() override;
    
    // Define helper functions and member variables privately
    private:
        ...
};
```


Implementation file: `src/static_request_handler_factory.cc`
```C++
// Include header file to implement
#include "static_request_handler_factory.h"
...

// Factory constructor should be passed all arguments necessary to configure object
// In this example, `StaticRequestHandler` configuration is parsed from an input NginxConfig 
StaticRequestHandlerFactory::StaticRequestHandlerFactory(const std::string& path, 
                                                         const NginxConfig& config) {
    ...
}

// createHandler should return a pointer to a StaticRequestHandler instance
std::shared_ptr<RequestHandler> StaticRequestHandlerFactory::createHandler() {
    ...
}

// Implement private function headers
...
```
Factory classes are used within the dispatcher logic to create `RequestHandler`s matched to incoming requests. Any logic to dynamically change handler behavior across requests of the same type should be called at factory construction time.

### 3. Link factory class in config parser
Once a factory class has been defined for the new request handler, it can be integrated into the server by linking it to the `createHandlerFactory` function. The entry point for the function is `src/config_parser.cc`.

Linking involves adding an `else if` block within `createHandlerFactory`. For example:

```C++
// File: (src/config_parser.cc)
...

// Function matches name of handler to factory instance
std::shared_ptr<RequestHandlerFactory> createHandlerFactory(
    const std::string& name, const std::string& uri,
    const NginxConfig& location) {
    if (name == "EchoHandler") {
        ...

    ...

    // Add new request handler within if-else chain
    else if (name == "StaticHandler") {
        // Return instance of factory, constructed with necessary arguments
        return std::make_shared<StaticRequestHandlerFactory>(uri, location);
    } 

    ...

    // Return the not found / 404 handler factory by default
    else {
        return std::make_shared<NotFoundHandlerFactory>(uri, location);
    }

...
```
Note that a string literal is associated to each `RequestHandlerFactory`. Our convention has been to use the inherited RequestHandler class name, with `"Request"` omitted to disambiguate between the class name and the string. i.e. `EchoRequestHandler` -> `"EchoHandler"`.

### 4. Update handler and handler_factory libraries in `CMakeLists.txt`
Once the new request handler has been integrated into the source code, the CMake build configuration must also be updated. Specifically, all inheritors of `RequestHandler` and `RequestHandlerFactory` are compiled within the `handler` and `handler_lib` libraries respectively. 
  


```python
# File: (CMakeLists.txt)

...
add_library(handler_factory_lib src/echo_request_handler_factory.cc ... src/static_request_handler_factory.cc)
add_library(handler_lib src/echo_request_handler.cc ... src/static_request_handler.cc)
...
```
Note that any internal or external dependencies introduced by a new request handler should be appropriately included within `CMakeLists.txt`. 

### 5. Write unit tests and add them to coverage
All new request handlers and factorys should have associated unit tests defined in the `tests` directory. Our naming convention for test files has been to append `"_test"` to the implementation filename. i.e. `static_request_handler.cc` -> `static_request_handler_test.cc`

Testing file: `tests/static_request_handler_test.cc`
```C++
// Include the class to test, as well as the gtest library
#include "static_request_handler.h"
#include "gtest/gtest.h"
...

class StaticRequestHandlerTest : public ::testing::Test {
    protected:
        void SetUp() override {}

        // Define handler within text fixture to avoid duplication
        StaticRequestHandler handler_ = StaticRequestHandler("static", "");
};

// Test default file not found behavior
TEST_F(StaticRequestHandlerTest, FileNotFound) {
    // Construct GET request
    http::request<http::string_body> req{http::verb::get, "/static/img/not_found", 11};
    http::response<http::string_body> res;

    // Test handle_request response population
    handler_.handle_request(req, res);
    std::stringstream res_stream;
    res_stream << res;
    std::string response = res_stream.str();

    // Compare to expected response string
    EXPECT_EQ(
        response,
        "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: "
        "87\r\n\r\n<html><head><title>404 Not Found</title></head><body><p>404 "
        "Not Found</p></body></html>");
}

// Other tests
...
```
Once unit tests for the new request handler have been written, they need to be compiled and linked to the coverage report. 


```python
# File: (CMakeLists.txt)

...
# Compile test executable, link required libraries, and expose to gtest
add_executable(static_request_handler_test tests/static_request_handler_test.cc)
target_link_libraries(static_request_handler_test ... handler_lib gtest_main)
gtest_discover_tests(static_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
...
# Add tests to coverage report
generate_coverage_report(TARGETS server ... TESTS config_parser_test ... static_request_handler_test static_request_handler_factory_test)
...
```