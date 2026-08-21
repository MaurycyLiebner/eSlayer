#include "eserverarea.h"

#include "actions/eclientaction.h"
#include "eitemgenerator.h"

#include <eSlayerHelpers/echaracter.h>
#include <eSlayerHelpers/eunitsinfo.h>
#include <eSlayerHelpers/echardatainfo.h>
#include <eSlayerHelpers/edifficulties.h>
#include <eSlayerHelpers/eclasses.h>
#include <eSlayerHelpers/eportals.h>

#include <eSlayerMapGenerator/emap.h>

void eServerArea::iniSetupSlayerAction(
    const std::shared_ptr<eServerUnit>& u) {
    const auto a = std::make_shared<eClientAction>(*u, *this);
    u->setAction(a);
}

bool eServerArea::addClient(const uint32_t clientId,
                            eCharacter& c,
                            eTeamId& teamId,
                            ePointF& spawnPos,
                            std::vector<eBody>& bodies,
                            const eScreenDimensions& screenDims) {
    auto& eq = c.equipment();
    const int classId = c.classId();
    const auto& class_ = eClasses::sClasses.get(classId);
    const auto uinfoId = class_.fUnitInfoId;
    const auto& udata = eUnitsInfo::sUnits.get(uinfoId);
    const auto& data = eCharDataInfo::get(udata.fCharData);
    auto& map = mMap->pathFinderMap();
    const auto u = std::make_shared<eServerUnit>(
        eUnitType::slayer, data, uinfoId, *this);
    eq.iterateOverAll([](eItem& item) {
        if(item.fType == eItemType::none) return;
        eItemGenerator::applyItemId(item);
    });
    u->setClass(c.classId());
    sSlayers[clientId] = u;
    u->addSkill();
    u->addSkill();
    spawnPos = mMap->spawnPos();
    findPlaceForUnit(spawnPos, spawnPos);
    teamId = eTeams::addTeam(clientId);
    const auto& modelParts = u->fModelParts;
    iniSetupUnit(u, clientId, teamId, spawnPos,
                 uinfoId, udata, data, modelParts);
    iniSetupSlayerAction(u);
    eq.iterateOverAll([](eItem& item) {
        if(item.fType == eItemType::none) return;
        eItemGenerator::applyItemId(item);
    });
    u->setEquipment(eq, false);
    const auto& attrs = c.attributes();
    u->setAttributes(attrs, false);
    const auto& skillLevels = c.skillLevels();
    u->setSkillLevels(skillLevels, false);
    u->recalculateStats();
    u->recalculateAuras();

    auto& clientData = mClientData[clientId];
    clientData.fLatestMissile = 0;
    clientData.fLatestNova = 0;
    clientData.fLatestSkillArea = 0;
    clientData.fScreen = screenDims;
    const auto area = unitArea(*u);
    clientData.fArea = area;
    const int diff = eDifficulties::sDifficulty;
    const auto& quests = c.quests(diff);
    clientData.fQuests = quests;
    clientData.fMerc = c.merc();

    for(auto& eq : c.bodies()) {
        eq.iterateOverBody([](eItem& item) {
            if(item.fType == eItemType::none) return;
            eItemGenerator::applyItemId(item);
        });
        auto& body = bodies.emplace_back();
        body.fMapId = mMap->id();
        body.fEq = eq;
        spawnBody(clientId, eq, body.fBodyId, body.fPos);
    }

    checkQuestItems(clientId);

    checkEnterArea(clientId);

    return true;
}

bool eServerArea::addClient(
    const uint32_t clientId,
    const std::shared_ptr<eServerUnit>& u,
    const std::vector<std::shared_ptr<eServerUnit>>& followers,
    const eClientData& srcData,
    const eMoveToMapData& moveData,
    ePointF& spawnPos) {
    switch(moveData.fType) {
    case eMoveToMapType::waypoint: {
        const auto& to = moveData.fTo;
        const auto toArea = to.fAreaId;
        const bool r = mMap->waypointPosition(
            toArea, spawnPos);
        if(!r) return false;
    } break;
    case eMoveToMapType::portal: {
        const auto pid = moveData.fPortalId;
        const auto p = ePortal::portal(pid);
        if(!p) return false;
        const bool camp = p->fCampPortalId == pid;
        spawnPos = camp ? p->fOutdoorPos : p->fCampPos;
    } break;
    case eMoveToMapType::entrance: {
        const bool r = mMap->spawnPos(
            moveData.fFrom, spawnPos);
        if(!r) return false;
    } break;
    case eMoveToMapType::spawn:
    case eMoveToMapType::respawn: {
        spawnPos = mMap->spawnPos();
        u->respawn();
    } break;
    }

    findPlaceForUnit(spawnPos, spawnPos);
    iniSetupUnit(u, spawnPos);
    iniSetupSlayerAction(u);
    u->setBlockingActionTime(0.f);
    u->updateAll();

    for(const auto& f : followers) {
        findPlaceForUnit(spawnPos, spawnPos);
        iniSetupUnit(f, spawnPos);
        iniSetupFollowerAction(f, u);
        f->setBlockingActionTime(0.f);
        f->updateAll();
    }

    auto& clientData = mClientData[clientId];
    clientData.fScreen = srcData.fScreen;
    clientData.fArea = unitArea(*u);
    clientData.fKnownUnits.clear();
    clientData.fKnownItems.clear();
    clientData.fLatestMissile = 0;
    clientData.fLatestNova = 0;
    clientData.fLatestSkillArea = 0;
    clientData.fKnownMap.clear();
    clientData.fUpdateBoostsAuras = srcData.fUpdateBoostsAuras;
    clientData.fQuests = srcData.fQuests;
    clientData.fSendQuests = srcData.fSendQuests;
    clientData.fMerc = srcData.fMerc;
    clientData.fFollowersState = srcData.fFollowersState;
    clientData.fUsedSkills = srcData.fUsedSkills;

    checkEnterArea(clientId);

    return true;
}