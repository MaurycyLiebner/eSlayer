#ifndef EREQUESTDATA_H
#define EREQUESTDATA_H

#include "eunitdata.h"
#include "emissile.h"
#include "enova.h"
#include "eskillarea.h"
#include "egrounditem.h"
#include "emapportion.h"
#include "eattributes.h"
#include "emercenary.h"

class ePacket;

struct ESLAYERHELPERS_API eRequestData {
    uint32_t fRequestId;
    uint32_t fServerState;
    std::vector<eUnitData> fNewUnits;
    std::vector<eUnitData> fUpdatedUnits;
    std::vector<eMissile> fMissiles;
    std::vector<eMissileUpdate> fMissileUpdates;
    std::vector<eNova> fNovas;
    std::vector<eSkillArea> fSkillAreas;
    std::vector<eGroundItem> fNewItems;
    std::vector<uint32_t> fRemovedItemIds;
    std::vector<eMapPortion> fMapPortions;
    bool fUpdateBoostsAuras;
    std::multimap<eBoostCurseType, eModifier> fBoosts;
    std::multimap<eAuraType, eModifier> fAuras;
    uint16_t fMana;
    uint16_t fStamina;
    std::optional<eAttributes> fAttributes;
    uint8_t fRemainingSkillPoints;
    std::optional<eMercenary> fMerc;
    std::set<int> fUsedSkills;

    bool read(ePacket& p, const uint32_t currentServerState);
    void write(ePacket& p) const;
};

#endif // EREQUESTDATA_H
