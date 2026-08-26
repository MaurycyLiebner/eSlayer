#ifndef ECLIENTDATA_H
#define ECLIENTDATA_H

#include "eservermissile.h"

#include <eSlayerHelpers/escreendimensions.h>
#include <eSlayerHelpers/eslayerquests.h>
#include <eSlayerHelpers/emercenary.h>
#include <eSlayerHelpers/eareas.h>
#include <eSlayerHelpers/earea.h>
#include <eSlayerHelpers/eidmapvector.h>

struct eClientData {
    eClientData();
    // in tile dimensions
    eScreenDimensions fScreen;
    eArea fArea;
    std::set<uint32_t> fKnownUnits;
    std::set<uint32_t> fKnownItems;
    std::set<int> fUsedSkills;
    eIdMapVector<eServerMissile> fRemovedMissiles;
    uint32_t fLatestMissile;
    uint32_t fLatestNova;
    uint32_t fLatestSkillArea;
    eAreas fKnownMap;
    bool fUpdateBoostsAuras = false;

    eSlayerQuests fQuests;
    bool fSendQuests = false;

    std::optional<eMercenary> fMerc;

    uint32_t fFollowersState = 0;
};

#endif // ECLIENTDATA_H
