#ifndef NOT_FOUND_HANDLER_FACTORY_H
#define NOT_FOUND_HANDLER_FACTORY_H

#include <memory>
#include <string>

#include "config_parser.h"
#include "request_handler.h"
#include "request_handler_factory.h"

class NotFoundHandlerFactory : public RequestHandlerFactory {
    public:
        NotFoundHandlerFactory(const std::string& path,
                               const NginxConfig& config);
        std::shared_ptr<RequestHandler> createHandler() override;
        std::string getHandlerName() override;

    private:
        void parseConfig() override;
        std::string path_;
        NginxConfig config_;
};

#endif