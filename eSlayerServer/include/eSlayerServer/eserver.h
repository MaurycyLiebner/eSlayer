#ifndef ESERVER_H
#define ESERVER_H

#include "eslayerserverexport.h"

#include "eserverunit.h"

#include <eSlayerMapGenerator/emapgenerator.h>

#include <memory>

class ESLAYERSERVER_API eServer {
public:
    virtual int connect() = 0;
    virtual bool disconnect(const int clientId) = 0;

    virtual void increment() = 0;

    virtual std::shared_ptr<eMap>
    requestMap(const int clientId,
               const std::string& name) = 0;

    virtual bool
    requestUnits(const int clientId) = 0;

    virtual int
    receiveUnits(const int clientId,
                 std::vector<std::shared_ptr<eServerUnit>>& units) = 0;
private:
};

namespace eSlayerServer {
    ESLAYERSERVER_API std::shared_ptr<eServer>
    generate(const std::string& name);
}

#endif // ESERVER_H
