#include "request_handler_factory.h"

void RequestHandlerFactory::setAddress(std::string addy) {
    address_ = std::move(addy);
}