#ifndef ESERVER_H
#define ESERVER_H

#include "eslayerserverexport.h"

#include <eSlayerMapGenerator/emapgenerator.h>

#include <memory>

class ESLAYERSERVER_API eServer {
  public:
    virtual std::shared_ptr<eMap>
    requestMap(const std::string& name) = 0;
  private:
};

namespace eSlayerServer {
    ESLAYERSERVER_API std::shared_ptr<eServer>
    generate(const std::string& name);
}

#endif // ESERVER_H
