#ifndef HEALTH_REQUEST_HANDLER_FACTORY_H
#define HEALTH_REQUEST_HANDLER_FACTORY_H

#include <memory>
#include <string>

#include "config_parser.h"
#include "request_handler.h"
#include "request_handler_factory.h"

class HealthRequestHandlerFactory : public RequestHandlerFactory {
    public:
        HealthRequestHandlerFactory(const std::string& path,
                                    const NginxConfig& config);
        std::shared_ptr<RequestHandler> createHandler() override;
        std::string getHandlerName() override;

    private:
        void parseConfig() override;
        std::string path_;
        NginxConfig config_;
};

#endif