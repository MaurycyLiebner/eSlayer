#ifndef EMAPGENERATOR_H
#define EMAPGENERATOR_H

#include "eslayermapgeneratorexport.h"

#include <memory>
#include <string>

class eMap;

namespace eSlayerMapGenerator {
    ESLAYERMAPGENERATOR_API std::shared_ptr<eMap>
    generate(const std::string& name);
}

#endif // EMAPGENERATOR_H
