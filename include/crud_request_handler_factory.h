#ifndef CRUD_REQUEST_HANDLER_FACTORY_H
#define CRUD_REQUEST_HANDLER_FACTORY_H

#include <memory>
#include <string>

#include "config_parser.h"
#include "file_system.h"
#include "request_handler.h"
#include "request_handler_factory.h"

class CRUDRequestHandlerFactory : public RequestHandlerFactory {
    public:
        CRUDRequestHandlerFactory(const std::string& path,
                                  const NginxConfig& config);
        std::shared_ptr<RequestHandler> createHandler() override;
        std::string getRoot();

    private:
        void parseConfig() override;
        std::string path_;
        std::string root_;
        NginxConfig config_;
        std::shared_ptr<FileSystem> file_system_;
};

#endif