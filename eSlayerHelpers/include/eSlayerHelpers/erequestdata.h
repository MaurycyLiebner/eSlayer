#ifndef EREQUESTDATA_H
#define EREQUESTDATA_H

#include "eunitdata.h"
#include "eunitdynamicdata.h"
#include "emissile.h"
#include "enova.h"
#include "eskillarea.h"
#include "egrounditem.h"
#include "emapportion.h"

class ePacket;

struct ESLAYERHELPERS_API eRequestData {
    uint32_t fRequestId;
    std::vector<eUnitData> fNewUnits;
    std::vector<eUnitDynamicData> fUpdatedUnits;
    std::vector<eDeadUnitDynamicData> fUpdatedDeadUnits;
    std::vector<eMissile> fMissiles;
    std::vector<eNova> fNovas;
    std::vector<eSkillArea> fSkillAreas;
    std::vector<eGroundItem> fNewItems;
    std::vector<uint32_t> fRemovedItemIds;
    std::vector<eMapPortion> fMapPortions;
    bool fUpdateBoostsAuras;
    std::multimap<eBoostCurseType, eModifier> fBoosts;
    std::multimap<eAuraType, eModifier> fAuras;
    uint16_t fMana;
    uint16_t fLevel;
    uint16_t fExperience;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EREQUESTDATA_H
