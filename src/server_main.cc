//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <signal.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>

#include "config_parser.h"
#include "logger.h"
#include "request_handler.h"
#include "server.h"
#include "session.h"

using boost::asio::ip::tcp;

void sigHandler(int signum) {
    Logger::log_info("Caught signal " + std::to_string(signum));
    Logger::log_info("Closing server");
    exit(0);
}

int main(int argc, char* argv[]) {
    // 10 * 1024 * 1024 = 10MB rotation size
    // 0, 0, 0 = 12:00:00 rotation time
    Logger::init_logger("../logs/SERVER_LOG_%N.log", 10 * 1024 * 1024,
                        {0, 0, 0});
    signal(SIGINT, &sigHandler);
    try {
        if (argc != 2) {
            std::cerr << "Usage: server <config>\n";
            Logger::log_error(
                "Incorrect usage found, correct usage: server <config>");
            return 1;
        }

        NginxConfig config;
        NginxConfigParser config_parser;
        bool success = config_parser.Parse(argv[1], &config);

        if (!success) {
            std::cerr << "Error parsing configuration file: " << argv[1]
                      << std::endl;
            Logger::log_error("Error parsing configuration file: " +
                              std::string(argv[1]));
            return -1;
        }

        int port = config.getPort();
        if (port == -1) {
            std::cerr << "Invalid port number: " << port << " provided"
                      << std::endl;
            Logger::log_error("Invalid port number: " + std::to_string(port) +
                              " provided");
            return -1;
        }

        std::map<std::string, std::shared_ptr<RequestHandler>> handlers =
            config.getHandlerMapping();

        boost::asio::io_service io_service;
        tcp::endpoint endpoint = tcp::endpoint(tcp::v4(), port);
        tcp::acceptor acceptor = tcp::acceptor(io_service, endpoint);

        server server(
            io_service, acceptor,
            [handlers](boost::asio::io_service& io_service) -> session* {
                tcp::socket socket(io_service);
                return new session(std::move(socket), handlers);
            });
        Logger::log_info("Server listening on port " + std::to_string(port));
        io_service.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        Logger::log_error("Exception: " + std::string(e.what()));
    }

    return 0;
}
