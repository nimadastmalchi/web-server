#ifndef SLEEP_REQUEST_HANDLER_FACTORY_H
#define SLEEP_REQUEST_HANDLER_FACTORY_H

#include <memory>
#include <string>

#include "config_parser.h"
#include "request_handler.h"
#include "request_handler_factory.h"

class SleepRequestHandlerFactory : public RequestHandlerFactory {
    public:
        SleepRequestHandlerFactory(const std::string& path,
                                   const NginxConfig& config);
        std::shared_ptr<RequestHandler> createHandler() override;
        int getSleepSeconds();

    private:
        void parseConfig() override;
        std::string path_;
        NginxConfig config_;
        int sleep_seconds_;
        static const int DEFAULT_MAX_SLEEP_SECONDS_ = 60;  // 1 minutes
};

#endif