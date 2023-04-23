//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "config_parser.h"
#include "server.h"
#include "session.h"

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
  try {
    if (argc != 2) {
      std::cerr << "Usage: server <config>\n";
      return 1;
    }

    NginxConfig config;
    NginxConfigParser config_parser;
    bool success = config_parser.Parse(argv[1], &config);

    if (!success) {
      std::cerr << "Error parsing config file: " << argv[1] << std::endl;
      return -1;
    }
  
    int port = config.getPort();
    if (port == -1) {
      std::cerr << "Invalid port number: " << port << " provided" << std::endl;
      return -1;
    }

    ResponseBuilder response_builder;

    boost::asio::io_service io_service;
    tcp::endpoint endpoint = tcp::endpoint(tcp::v4(), port);
    tcp::acceptor acceptor = tcp::acceptor(io_service, endpoint);
    
    server s(io_service, acceptor, response_builder);
    io_service.run();
  }
  catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
