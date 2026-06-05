#ifndef EUNITDATA_H
#define EUNITDATA_H

#include "eslayerhelpersexport.h"

#include "echardata.h"
#include "eunitdynamicdata.h"
#include "eteamid.h"

class ePacket;

const uint8_t sFleshExplAnim = 255;
const uint8_t sIceExplAnim = 254;

struct ESLAYERHELPERS_API eUnitData :
    public eUnitDynamicData {
    eTeamId fTeamId;

    uint8_t fCharDataId;

    float fRadius;

    std::set<uint8_t> fMods;

    eModelParts fModelParts;

    void removeBoostData(const uint8_t id);
    void addBoostData(const uint8_t id);

    eUnitData toUnitData() const;
    eUnitDynamicData toDynamicData() const;

    void read(ePacket& p);
    void write(ePacket& p) const;

    bool cold() const;
    void setCold(const bool c);

    bool frozen() const;
    void setFrozen(const bool f);

    bool poisoned() const;
    void setPoisoned(const bool p);

    static float sColdSpeed;
};

#endif // EUNITDATA_H
