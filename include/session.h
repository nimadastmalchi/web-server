#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>

#include "request_handler_factory.h"

using boost::asio::ip::tcp;

class session {
    public:
        session(tcp::socket socket,
                std::map<std::string, std::shared_ptr<RequestHandlerFactory>>
                    handlerFactories);
        tcp::socket& socket();
        void start();

        friend class SessionTest;

    private:
        int handle_read(const boost::system::error_code& error,
                        size_t bytes_transferred);
        int close_socket(const boost::system::error_code& error);
        std::shared_ptr<RequestHandlerFactory> get_handler_factory(
            const boost::beast::http::request<boost::beast::http::string_body>&
                req);
        void log_metrics_info(int response_code,
                              const std::string& request_path,
                              const std::string& handler_name);

        tcp::socket socket_;

        std::string responseStr_;

        int bytes_read_;
        // Max length constant chosen to support 64K HTTP request size
        // Per: https://piazza.com/class/lfx37nmbiup24h/post/118
        enum { max_length = 64000 };
        char data_[max_length];

        std::map<std::string, std::shared_ptr<RequestHandlerFactory>>
            handlerFactories_;
};

#endif