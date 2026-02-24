#ifndef ESERVERAREA_H
#define ESERVERAREA_H

#include "eSlayerServer/eserverunit.h"

#include <eSlayerHelpers/emovementhandler.h>

#include <memory>

class eServerArea {
public:
    void increment();

    const std::vector<std::shared_ptr<eServerUnit>>&
    units() const { return mUnits; }
private:
    std::vector<std::shared_ptr<eServerUnit>>
    mUnits;
    std::vector<std::shared_ptr<eMovementHandler>>
    mMovementHandlers;
};

#endif // ESERVERAREA_H
