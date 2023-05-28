#ifndef STATIC_REQUEST_HANDLER_FACTORY_H
#define STATIC_REQUEST_HANDLER_FACTORY_H

#include <memory>
#include <string>

#include "config_parser.h"
#include "request_handler.h"
#include "request_handler_factory.h"

class StaticRequestHandlerFactory : public RequestHandlerFactory {
    public:
        StaticRequestHandlerFactory(const std::string& path,
                                    const NginxConfig& config);
        std::shared_ptr<RequestHandler> createHandler() override;
        std::string getHandlerName() override;
        std::string getRoot();

    private:
        void parseConfig() override;
        std::string path_;
        std::string root_;
        NginxConfig config_;
};

#endif