#ifndef ESERVERAREA_H
#define ESERVERAREA_H

#include "eserverunit.h"

#include <eSlayerMapGenerator/emapgenerator.h>
#include <eSlayerHelpers/emovementhandler.h>

#include <memory>
#include <map>

class eServerArea {
public:
    void initialize(const std::shared_ptr<eMap>& map);

    void increment();

    const std::vector<std::shared_ptr<eServerUnit>>&
    units() const { return mUnits; }

    void addClient(const int clientId, const ePointF& pos);

    std::shared_ptr<eServerUnit>
    unit(const int charId) const;
private:
    std::vector<std::shared_ptr<eServerUnit>>
    mUnits;
    std::map<int, int> mUnitIdMap;

    std::shared_ptr<eMap> mMap;
};

#endif // ESERVERAREA_H
