#ifndef CRUD_REQUEST_HANDLER_FACTORY_H
#define CRUD_REQUEST_HANDLER_FACTORY_H

#include <string>

#include "config_parser.h"
#include "request_handler.h"
#include "request_handler_factory.h"

class CRUDRequestHandlerFactory : public RequestHandlerFactory {
    public:
        CRUDRequestHandlerFactory(const std::string& path,
                                  const NginxConfig& config);
        std::shared_ptr<RequestHandler> createHandler() override;

    private:
        void parseConfig() override;
        std::string path_;
        std::string root_;
        NginxConfig config_;
};

#endif