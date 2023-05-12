#ifndef REQUEST_HANDLER_FACTORY_H
#define REQUEST_HANDLER_FACTORY_H

#include <iostream>
#include <memory>
#include <string>

#include "request_handler.h"

class RequestHandlerFactory {
    public:
        virtual std::shared_ptr<RequestHandler> createHandler() = 0;

    private:
        virtual void parseConfig() = 0;
};

#endif