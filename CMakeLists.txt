cmake_minimum_required(VERSION 3.10.0)
project(a2-team-please-adopt-me-nima)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS log_setup log system filesystem)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)

# Compile and link server executable
add_library(config_parser_lib src/config_parser.cc)
add_library(file_system_lib src/file_system.cc)
add_library(handler_factory_lib src/file_system.cc src/echo_request_handler_factory.cc src/static_request_handler_factory.cc src/not_found_handler_factory.cc src/crud_request_handler_factory.cc src/sleep_request_handler_factory.cc)
add_library(handler_lib src/echo_request_handler.cc src/static_request_handler.cc src/not_found_handler.cc src/crud_request_handler.cc src/sleep_request_handler.cc)
add_library(logger_lib src/logger.cc)
add_library(server_lib src/server.cc src/session.cc)
add_executable(server src/server_main.cc)
target_link_libraries(server server_lib config_parser_lib handler_factory_lib handler_lib file_system_lib logger_lib Boost::system Boost::log_setup Boost::log Boost::filesystem)

# Compile and link tests
# Unit Tests (runs via `make test`)
add_executable(config_parser_test tests/config_parser_test.cc)
target_link_libraries(config_parser_test config_parser_lib handler_factory_lib handler_lib logger_lib gtest_main Boost::log_setup Boost::log)
gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_executable(session_test tests/session_test.cc)
target_link_libraries(session_test config_parser_lib server_lib handler_factory_lib handler_lib logger_lib gtest_main Boost::log_setup Boost::log)
gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_executable(server_test tests/server_test.cc)
target_link_libraries(server_test config_parser_lib server_lib handler_factory_lib handler_lib logger_lib gtest_main Boost::log_setup Boost::log)
gtest_discover_tests(server_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_executable(logger_test tests/logger_test.cc)
target_link_libraries(logger_test logger_lib gtest_main Boost::log_setup Boost::log)
gtest_discover_tests(logger_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_executable(echo_request_handler_test tests/echo_request_handler_test.cc)
target_link_libraries(echo_request_handler_test server_lib handler_lib gtest_main)
gtest_discover_tests(echo_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_executable(static_request_handler_test tests/static_request_handler_test.cc)
target_link_libraries(static_request_handler_test config_parser_lib server_lib handler_lib logger_lib gtest_main Boost::filesystem Boost::log_setup Boost::log)
gtest_discover_tests(static_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_executable(not_found_handler_test tests/not_found_handler_test.cc)
target_link_libraries(not_found_handler_test config_parser_lib server_lib handler_lib gtest_main)
gtest_discover_tests(not_found_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_executable(crud_request_handler_test tests/crud_request_handler_test.cc)
target_link_libraries(crud_request_handler_test config_parser_lib server_lib handler_lib file_system_lib logger_lib gtest_main Boost::filesystem Boost::log_setup Boost::log)
gtest_discover_tests(crud_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_executable(sleep_request_handler_test tests/sleep_request_handler_test.cc)
target_link_libraries(sleep_request_handler_test config_parser_lib server_lib handler_lib file_system_lib logger_lib gtest_main Boost::filesystem Boost::log_setup Boost::log)
gtest_discover_tests(sleep_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_executable(file_system_test tests/file_system_test.cc)
target_link_libraries(file_system_test file_system_lib gtest_main Boost::filesystem Boost::log_setup Boost::log)
gtest_discover_tests(file_system_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_executable(static_request_handler_factory_test tests/static_request_handler_factory_test.cc)
target_link_libraries(static_request_handler_factory_test config_parser_lib server_lib handler_factory_lib handler_lib logger_lib gtest_main Boost::filesystem Boost::log_setup Boost::log)
gtest_discover_tests(static_request_handler_factory_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_executable(crud_request_handler_factory_test tests/crud_request_handler_factory_test.cc)
target_link_libraries(crud_request_handler_factory_test config_parser_lib server_lib handler_factory_lib handler_lib logger_lib gtest_main Boost::filesystem Boost::log_setup Boost::log)
gtest_discover_tests(crud_request_handler_factory_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_executable(sleep_request_handler_factory_test tests/sleep_request_handler_factory_test.cc)
target_link_libraries(sleep_request_handler_factory_test config_parser_lib server_lib handler_factory_lib handler_lib logger_lib gtest_main Boost::filesystem Boost::log_setup Boost::log)
gtest_discover_tests(sleep_request_handler_factory_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# Integration Tests (runs via `make test`)
find_program(BASH_PROGRAM bash)
add_test(NAME integration_test COMMAND ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_test.sh)

# End to end Tests (runs via `make e2e`)
add_custom_target(e2e
  COMMAND ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/tests/e2e_test.sh
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Running end-to-end tests"
)

# Init the git hooks 
add_custom_target(init_githooks
  COMMAND git config core.hooksPath githooks && chmod +x githooks/*
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Initialize git hooks folder"
)

include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS server file_system_lib server_lib config_parser_lib logger_lib handler_lib TESTS config_parser_test logger_test file_system_test session_test server_test echo_request_handler_test static_request_handler_test not_found_handler_test static_request_handler_factory_test crud_request_handler_factory_test crud_request_handler_test sleep_request_handler_factory_test sleep_request_handler_test)
