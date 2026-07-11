#ifndef ESERVERAREA_H
#define ESERVERAREA_H

#include "eserverunit.h"
#include "eservermissile.h"
#include "eservernova.h"
#include "eserverskillarea.h"

#include <eSlayerMissiles/emissileincrementer.h>
#include <eSlayerMissiles/enovaincrementer.h>

#include <eSlayerHelpers/emovementhandlerbase.h>
#include <eSlayerHelpers/eidmapvector.h>
#include <eSlayerHelpers/earea.h>
#include <eSlayerHelpers/eareas.h>
#include <eSlayerHelpers/esetareas.h>
#include <eSlayerHelpers/escreendimensions.h>
#include <eSlayerHelpers/efixedsizesetareas.h>
#include <eSlayerHelpers/ebody.h>
#include <eSlayerHelpers/eequipmentaction.h>
#include <eSlayerHelpers/eslayerquests.h>
#include <eSlayerHelpers/etalk.h>
#include <eSlayerHelpers/emercenary.h>

#include <eSlayerMapGenerator/emapgenerator.h>

#include <memory>
#include <map>
#include <set>

class eCharacter;
struct eUnitInfo;
struct eServerDoors;
struct eMoveToMapData;
struct eSeller;

struct eClientData {
    eClientData();
    // in tile dimensions
    eScreenDimensions fScreen;
    eArea fArea;
    std::set<uint32_t> fKnownUnits;
    std::set<uint32_t> fKnownItems;
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

class eServerArea {
public:
    eServerArea();

    void initialize(const std::shared_ptr<eMap>& map);
    const std::shared_ptr<eMap>& map() const { return mMap; }

    void increment(const float by = 1.f);

    float time() const { return mTime; }

    const eIdMapVector<eServerUnit>&
    units() const { return mUnits; }

    void unitsData(const uint32_t clientId,
                   std::vector<eUnitData>& newUnits,
                   std::vector<eUnitData>& updatedUnits);
    void itemsData(const uint32_t clientId,
                   std::vector<eGroundItem>& newItems,
                   std::vector<uint32_t>& removedItemIds);
    std::vector<eMissile>
    missileData(const uint32_t clientId);
    std::vector<eNova>
    novaData(const uint32_t clientId);
    std::vector<eSkillArea>
    skillAreaData(const uint32_t clientId);
    bool boostsAurasChanged(const uint32_t clientId);
    bool updateBoostsAuras(const uint32_t clientId);
    std::multimap<eBoostCurseType, eModifier>
    boosts(const uint32_t clientId);
    std::multimap<eAuraType, eModifier>
    auras(const uint32_t clientId);
    std::optional<eSlayerQuests>
    quests(const uint32_t clientId);
    std::optional<eMercenary>
    merc(const uint32_t clientId);

    bool heardTalk(const uint32_t clientId,
                   const eConvoId& talk);
    bool addedSocket(const uint32_t clientId,
                     const uint8_t questId);

    eArea unitArea(const uint32_t charId) const;
    eArea unitArea(const eServerUnit& u) const;

    eArea itemArea(const uint32_t itemId) const;
    eArea itemArea(const eGroundItem& i) const;

    eArea itemTile(const uint32_t itemId) const;
    eArea itemTile(const eGroundItem& i) const;

    bool mapPortions(const uint32_t clientId,
                     std::vector<eMapPortion>& result);

    bool walkable(const ePointF& pos) const;
    bool walkable(const ePointF& from,
                  const ePointF& to) const;
    bool obstacle(const ePointF& pos) const;
    bool obstacle(const ePointF& from,
                  const ePointF& to) const;

    bool addClient(const uint32_t clientId,
                   eCharacter& c,
                   eTeamId& teamId,
                   ePointF& spawnPos,
                   std::vector<eBody>& bodies,
                   const eScreenDimensions& screenDims);
    bool createBody(const uint32_t clientId, eBody& body);
    bool removeClient(const uint32_t clientId);
    bool clientMoved(const uint32_t clientId);
    bool planRemoveUnit(const uint32_t charId);
    bool removeUnit(const uint32_t charId);
    bool pickupBody(const uint32_t clientId,
                    const uint32_t bodyId,
                    bool& bodyRemoved,
                    eBodyItemsTaken& taken);
    bool changeTeam(const uint32_t clientId,
                    const eTeamId newTeam);

    bool spawnPortal(const uint32_t clientId,
                     uint32_t& portalId,
                     eAreaIds& area,
                     ePointF& pos);
    bool spawnCampPortal(const uint32_t clientId,
                         uint32_t& portalId,
                         eAreaIds& area,
                         ePointF& pos);

    bool triggerObject(
        const uint32_t clientId, eServerObject& obj);

    bool triggerDoors(const uint32_t clientId,
                      const eServerDoors& doors);

    bool pickupItem(const uint32_t clientId,
                    const uint32_t itemId,
                    const bool drag,
                    eEquipmentAction& action);
    bool dropItem(const uint32_t clientId);
    bool dropGold(const uint32_t clientId,
                  uint32_t count);
    void rearrangeItems(const uint32_t clientId,
                        const eEquipment& eq);
    bool equipmentAction(const uint32_t clientId,
                         const eEquipmentAction& a);
    bool buyAction(const uint32_t clientId,
                   const eBuyAction& a,
                   uint32_t& newItemId);
    bool sellAction(const uint32_t clientId,
                   const eSellAction& a);
    void changeAttributes(const uint32_t clientId,
                          const eAttributes& attrs);
    void changeSkillLevels(const uint32_t clientId,
                           const eSkillLevels& skillLevels);
    void consumePotion(const uint32_t clientId,
                       const uint32_t itemId,
                       const uint32_t unitId);

    void addSkillArea(const std::shared_ptr<eServerSkillArea>& a);
    void addMissile(const std::shared_ptr<eServerMissile>& m);
    void addNova(const std::shared_ptr<eServerNova>& n);

    void spawnMissile(const ePointF& to,
                      const eSkill& skill,
                      const eHitData& data,
                      const int nMissiles,
                      const float pierceChance,
                      const int missileId,
                      const float range,
                      const float radius,
                      const float time,
                      const bool continuousDamage);
    void spawnArea(const ePointF& to,
                   const eSkill& skill,
                   const eHitData& data,
                   const float radius,
                   const int missileId);
    void spawnNova(const eSkill& skill,
                   const eHitData& data,
                   const float radius,
                   const bool continuousDamage);
    void summon(eServerUnit& by,
                ePointF to,
                const int unitId,
                const int maxCount,
                const std::vector<eModifier>& mods);
    bool summonMerc(const uint32_t clientId,
                    eMercenary merc);
    bool castChance(eServerUnit& by,
                    const eSkillStats& skill,
                    const eWeaponChoice wchoice,
                    const ePointF& to);
    void cast(eServerUnit& by,
              const eSkillStats& skill,
              const eWeaponChoice wchoice,
              const ePointF& to);
    std::vector<uint32_t> summoned(const eServerUnit& by,
                                   const int unitId);

    std::shared_ptr<eServerUnit>
    unit(const uint32_t charId) const;
    std::shared_ptr<eGroundItem> groundItem(
        const uint32_t itemId) const;
    using eValidator = std::function<bool(const eServerUnit&)>;
    std::shared_ptr<eServerUnit> unit(
        const ePointF& pos,
        const eValidator& validator = nullptr) const;
    using eUnitIter = std::function<bool(const std::shared_ptr<eServerUnit>&)>;
    bool iterateOverUnits(const eArea& areaMin,
                          const eArea& areaMax,
                          const eUnitIter& iter) const;
    bool iterateOverUnits(const ePointF& pos,
                          const float maxRadius,
                          const eUnitIter& iter) const;

    void unitKilled(const eServerUnit& killed);

    static bool moveClient(const uint32_t clientId,
                           eServerArea& from,
                           eServerArea& to,
                           const eMoveToMapData& moveData,
                           ePointF& spawnPos);
    bool findPlaceForUnit(const ePointF& pos,
                          ePointF& result) const;
    bool findPlaceForPortal(const ePointF& pos,
                            ePointF& result) const;

    bool requestSeller(const uint32_t clientId,
                       const uint32_t sellerId,
                       eSeller& seller);

    std::optional<eFollowersBase> followersUpdate(
        const uint32_t clientId);

    static std::map<uint32_t, std::shared_ptr<eServerUnit>> sSlayers;
    static std::vector<uint32_t> sSlain;
private:
    bool checkQuestItems(
        const uint32_t clientId);

    bool spawnPortal(ePointF& pos,
                     uint32_t& portalId,
                     eAreaIds& area);
    bool spawnBody(const uint32_t clientId,
                   const eBodyEquipment& beq,
                   uint32_t& bodyId,
                   ePointF& spawnPos);
    bool addClient(const uint32_t clientId,
                   const std::shared_ptr<eServerUnit>& u,
                   const std::vector<std::shared_ptr<eServerUnit>>& followers,
                   const eClientData& srcData,
                   const eMoveToMapData& moveData,
                   ePointF& spawnPos);
    void iniMissileInc();
    void iniNovaInc();

    void removePlannedUnits();
    void iniSetupUnit(const std::shared_ptr<eServerUnit>& u,
                      const uint32_t charId,
                      const eTeamId teamId,
                      const ePointF& pos,
                      const uint8_t unitInfoId,
                      const eUnitInfo& uinfo,
                      const eCharData& data,
                      const eModelParts& modelParts);
    void iniSetupUnit(const std::shared_ptr<eServerUnit>& u,
                      const ePointF& pos);
    void iniSetupSlayerAction(
        const std::shared_ptr<eServerUnit>& u);
    void iniSetupFollowerAction(
        const std::shared_ptr<eServerUnit>& u,
        const std::shared_ptr<eServerUnit>& follow);
    void addGroundItem(const ePointF& pos,
                       const eItem& item);
    void generateItems(const ePointF& pos,
                       const int level,
                       const float worth);
    void generateItem(const ePointF& pos,
                      const int level,
                      const float worth);
    void generatePotion(const ePointF& pos,
                        const int level,
                        const float worth);

    float mTime = 0.f;

    std::shared_ptr<eMap> mMap;

    eIdMapVector<eServerMissile> mMissiles;
    eIdMapVector<eServerSkillArea> mSkillAreas;
    eIdMapVector<eServerNova> mNovas;
    eIdMapVector<eServerUnit> mUnits;
    eIdMapVector<eItem> mItemsOnGround;
    eIdMapVector<eGroundItem> mGroundItems;

    const int mUnitAreaDim = 4;
    const int mUnitAreaMargin = 3;
    eFixedSizeSetAreas mUnitAreas;
    const int mAuraRecalcSpan = 12;
    int mAuraRecalcCounter = 0;
    std::set<int> mAuraSources;
    const int mItemAreaDim = 4;
    eFixedSizeSetAreas mItemAreas;
    const int mItemTileSubdivision = 2;
    eFixedSizeSetAreas mItemTiles;

    std::vector<uint32_t> mUnitsToRemove;
    std::map<uint32_t, eClientData> mClientData;
    std::map<uint32_t, std::vector<uint32_t>> mBodies;

    eMissileIncrementer mMIncrementer;
    eNovaIncrementer mNIncrementer;

    std::set<uint32_t> mPortals;
    uint16_t mPortalsVersion = 0;
};

#endif // ESERVERAREA_H
