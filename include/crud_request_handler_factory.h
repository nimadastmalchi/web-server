#ifndef CRUD_REQUEST_HANDLER_FACTORY_H
#define CRUD_REQUEST_HANDLER_FACTORY_H

#include <string>

#include "config_parser.h"
#include "entity_manager.h"
#include "file_system.h"
#include "request_handler.h"
#include "request_handler_factory.h"

class CRUDRequestHandlerFactory : public RequestHandlerFactory {
    public:
        CRUDRequestHandlerFactory(const std::string& path,
                                  const NginxConfig& config);
        ~CRUDRequestHandlerFactory();
        std::shared_ptr<RequestHandler> createHandler() override;
        std::string getRoot();

    private:
        void parseConfig() override;
        std::string path_;
        std::string root_;
        NginxConfig config_;
        FileSystem* fs;
        EntityManager entity_manager_;
};

#endif