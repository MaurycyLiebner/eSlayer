#include "egamewidget.h"

#include "eskillbutton.h"
#include "esellerwidget.h"
#include "equestswidget.h"

#include "eunitindicator.h"
#include "ehoverwidget.h"
#include "einventorywidget.h"
#include "eminimap.h"

#include "../../emainwindow.h"
#include "../../screens/egamescreen.h"

#include "../../textures/eobjstextures.h"
#include "../../textures/eterrstextures.h"
#include "../../textures/emissilestextures.h"
#include "../../textures/etextgenerator.h"
#include "../../textures/eiteminstancetexture.h"
#include "../../textures/eitemstextures.h"
#include "../../textures/euitextures.h"

#include "../../names/eareanames.h"
#include "../../names/eobjectnames.h"
#include "../../names/emonsternames.h"
#include "../../names/emercenarynames.h"

#include "../../etext.h"

#include <eSlayerMissiles/emissileincrement.h>

#include <eSlayerHelpers/egamedir.h>
#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/earea.h>
#include <eSlayerHelpers/evec2.h>
#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/echaracter.h>
#include <eSlayerHelpers/estringhelpers.h>
#include <eSlayerHelpers/eobjectsinfo.h>
#include <eSlayerHelpers/eunitsinfo.h>
#include <eSlayerHelpers/eelitemodifiersinfo.h>
#include <eSlayerHelpers/erunsettings.h>
#include <eSlayerHelpers/especialanim.h>
#include <eSlayerHelpers/eslayers.h>
#include <eSlayerHelpers/ewaypoints.h>
#include <eSlayerHelpers/eportals.h>
#include <eSlayerHelpers/esellers.h>
#include <eSlayerHelpers/emercenaries.h>
#include <eSlayerHelpers/evectorhelpers.h>
#include <eSlayerHelpers/edifficulties.h>

eGameWidget* eGameWidget::sInstance = nullptr;
std::vector<std::string> eGameWidget::sMessageLog;

eGameWidget::eGameWidget(eMainWindow* const window) :
    eLabel(window),
    mWorld(mMap),
    mInput(resolution().tileWidth(),
           resolution().tileHeight()),
    mGamePainter(mTileIterator, renderer()) {
    sInstance = this;
    setNoPadding();
}

eGameWidget::~eGameWidget() {
    sInstance = nullptr;
    if(mServer) {
        save();
    }
}

void eGameWidget::initialize(const uint32_t clientId,
                             const std::shared_ptr<eServer>& server,
                             const std::shared_ptr<eMap>& map,
                             const eCharacter& c,
                             const eTeamId teamId,
                             const eMoveToMapAction& move) {
    mC = c;
    mCName = c.name();
    mHardcore = c.hardcore();

    eSlayers::sThisSlayer = clientId;
    mClientId = clientId;
    mServer = server;
    mMap = map;

    mMoveAction = move;

    initializeTextures();

    const auto r = renderer();
    const auto& res = resolution();

    const auto iter = [this](const ePointF& pos,
                             const float dist,
                             const eOtherHandler& handler) {
        const auto mapId = mMap->id();
        for(const auto& u : mWorld.units()) {
            if(!u) continue;
            if(u->fMapId != mapId) continue;
            handler(*u);
        }
    };
    auto& pathFinderMap = map->pathFinderMap();
    mMainAction = std::make_shared<eMainCharAction>(pathFinderMap);
    mMainAction->initialize(mServer, res, r, mMap, iter,
                            clientId, teamId, c.classId());
    mMainChar = mMainAction->unit();
    mMainChar->fPos = map->spawnPos();

    setRightSkill(c.rightSkill());
    setLeftSkill(c.leftSkill());

    const bool run = c.running();
    mMainAction->setRunning(run);

    const auto& srcEq = c.equipment();
    auto& dstEq = mMainAction->equipment();
    dstEq = srcEq;

    const auto& srcAttrs = c.attributes();
    auto& dstAttrs = mMainAction->attributes();
    dstAttrs = srcAttrs;

    auto& dstStats = mMainAction->stats();
    dstStats.fBaseSkillLevels = c.skillLevels();
    dstStats.calculate(dstAttrs, dstEq);
    dstStats.calculateAuras(dstEq);

    const int diff = eDifficulties::sDifficulty;

    const auto& srcQuests = c.quests(diff);
    auto& dstQuests = mMainAction->quests();
    dstQuests = srcQuests;

    const auto& srcTalkHeard = c.talkHeard(diff);
    auto& dstTalkHeard = mMainAction->talkHeard();
    dstTalkHeard = srcTalkHeard;

    const auto& srcWaypoints = c.waypoints(diff);
    auto& dstWaypoints = mMainAction->waypoints();
    dstWaypoints = srcWaypoints;

    const auto& srcMerc = c.merc();
    auto& dstMerc = mMainAction->merc();
    dstMerc = srcMerc;
    if(dstMerc && !dstMerc->fDead) {
        mServer->summonMerc(mClientId, *dstMerc);
    }

    mWorld.initialize(clientId, mMainChar);

    const int fontSize = res.smallFontSize();
    const auto font = eFonts::textFont(fontSize);
    mItemNames.initialize(r, font);

    mTileIterator.initialize(this);
}

const ePointF& eGameWidget::characterPos() const {
    return mMainAction->pos();
}

ePointF eGameWidget::pixelToTilePos(
    const ePointF& pos,
    const ePointF& pixel) const {
    return mInput.pixelToTilePos(pos, pixel, width(), height());
}

ePointF eGameWidget::pixelToTilePos(
    const ePointF& pixel) const {
    const auto& pos = characterPos();
    return mInput.pixelToTilePos(pos, pixel, width(), height());
}

ePointF eGameWidget::tilePosToPixel(const ePointF& pos) const {
    const auto& charPos = characterPos();
    return mInput.tilePosToPixel(pos, charPos, width(), height());
}

ePoint eGameWidget::tilePosToIPixel(const ePointF& pos) const {
    const auto iPos = pos.floor();
    auto pixel = tilePosToPixel(iPos);
    pixel = pixel.round();
    const float dx = pos.fX - iPos.fX;
    const float dy = pos.fY - iPos.fY;
    const int tileW = tileWidth();
    const int tileH = tileHeight();
    pixel.fX += (dx - dy)*(tileW/2);
    pixel.fY += (dx + dy)*((tileH + 1)/2);
    return pixel.round();
}

void eGameWidget::setUnitIndicator(eUnitIndicator* const indicator) {
    mUnitIndicator = indicator;
}

void eGameWidget::stop() {
    mMainAction->stop();
    mMainAction->stand();
}

void eGameWidget::dropItem() {
    auto& eq = mMainAction->equipment();
    auto& dragged = eq.fDragged;
    auto& tmp = eq.fTemporary;
    if(dragged.fType == eItemType::none &&
       tmp.fType == eItemType::none) return;
    mServer->dropItem(mClientId);
    dragged = eItem();
    tmp = eItem();
    eHoverWidget::sUpdateDragItem(dragged);
    updateWantsToTalk();
}

void eGameWidget::dropGold(const int count) {
    mServer->dropGold(mClientId, count);
}

void eGameWidget::sendEqAction(const eEquipmentAction& a) {
    mServer->equipmentAction(mClientId, a);
    mMainAction->recalculateStats();
}

void eGameWidget::sendBuyAction(
    const eBuyAction& a) {
    mServer->buyAction(mClientId, a);
}

void eGameWidget::sendSellAction(
    const eSellAction& a) {
    mServer->sellAction(mClientId, a);
}

void eGameWidget::sendAttributesChanged() {
    const auto& attrs = mMainAction->attributes();
    mServer->changeAttributes(mClientId, attrs);
}

void eGameWidget::sendSkillLevelsChanged() {
    const auto& stats = mMainAction->stats();
    const auto& skillLevels = stats.fBaseSkillLevels;
    mServer->changeSkillLevels(mClientId, skillLevels);
}

bool eGameWidget::dropPortrait(const uint32_t unitId) {
    auto& merc = eGameWidget::merc();
    if(!merc) return false;
    if(merc->fUnitId != unitId) return false;
    auto& eq = equipment();
    auto& meq = merc->fEq;
    auto& dragged = eq.fDragged;
    if(dragged.fType == eItemType::none) return false;
    if(dragged.fType == eItemType::potion) {
        const bool r = consumePotion(dragged, unitId);
        if(r) eq.take(dragged.fItemId);
        return r;
    } else {
        const auto mtype = merc->fMercType;
        const auto& m = eMercenariesInfo::sMercs.get(mtype);
        const auto& eqO = m.fEq;
        const bool r = eqO.validateItem(dragged);
        if(!r) return false;
        for(const auto type : eqO.fEquipment) {
            eEquipmentPlace p;
            const bool r = meq.tryAdd(dragged, type, &p);
            if(!r) continue;
            dragged = eItem();
            eEquipmentAction a;
            a.fType = eEquipmentActionType::drop;
            a.fUnitId = unitId;
            a.fPlace = p;
            eGameWidget::sSendEqAction(a);
            return true;
        }
    }
    return false;
}

void eGameWidget::setLeftSkill(const int s) {
    if(mLeftSkill == s) return;
    mLeftSkill = s;
    mMainAction->setSkillId(eSkillChoice::left, s);
    mServer->setSkillId(mClientId, eSkillChoice::left, s);
}

void eGameWidget::setRightSkill(const int s) {
    if(mRightSkill == s) return;
    mRightSkill = s;
    mMainAction->setSkillId(eSkillChoice::right, s);
    mServer->setSkillId(mClientId, eSkillChoice::right, s);
}

void eGameWidget::setOtherLeftSkill(const int s) {
    mOtherLeftSkill = s;
}

void eGameWidget::setOtherRightSkill(const int s) {
    mOtherRightSkill = s;
}

void eGameWidget::respawn() {
    auto& eq = equipment();
    eq.takeBody();
    eq.fInventoryGold = 0;
    mServer->createBody(mClientId);

    eMoveToMapData move;
    move.fType = eMoveToMapType::respawn;
    mMoveAction(move);
}

bool eGameWidget::switchRunning() {
    const bool run = !mMainAction->running();
    mMainAction->setRunning(run);
    return run;
}

bool eGameWidget::switchWeapons() {
    auto& eq = mMainAction->equipment();
    eq.fWeapons1 = !eq.fWeapons1;

    eEquipmentAction a;
    a.fType = eEquipmentActionType::switchWeapons;
    a.fUnitId = mClientId;
    a.fWeapons1 = eq.fWeapons1;
    eGameWidget::sSendEqAction(a);

    return eq.fWeapons1;
}

void eGameWidget::save() {
    const auto c = character();
    const auto path = eGameDir::path(
        "Save/" + mCName + ".xml");
    c.write(path);
}

eCharacter eGameWidget::character() {
    auto c = mC;
    const auto& questsSrc = quests();
    const int diff = eDifficulties::sDifficulty;
    if(questsSrc.difficultyFinished()) {
        int& latest = c.latestDifficulty();
        if(diff >= latest) {
            const int max = eDifficulties::sDifficulties.size() - 1;
            latest = std::min(max, diff + 1);
        }
    }
    c.setRunning(running());
    c.equipment() = equipment();
    c.attributes() = attributes();
    c.quests(diff) = questsSrc;
    c.talkHeard(diff) = talkHeard();
    const auto& stats = eGameWidget::stats();
    c.skillLevels() = stats.fBaseSkillLevels;
    c.leftSkill() = mLeftSkill;
    c.rightSkill() = mRightSkill;
    c.otherLeftSkill() = mOtherLeftSkill;
    c.otherRightSkill() = mOtherRightSkill;
    c.leftHotkeys() = eSkillButton::sLeftMap;
    c.rightHotkeys() = eSkillButton::sRightMap;

    auto& bodies = c.bodies();
    bodies.clear();
    for(const auto& b : eBodies::sBodies) {
        bodies.emplace_back(b.fEq);
    }

    c.waypoints(diff) = eGameWidget::waypoints();
    c.merc() = merc();
    return c;
}

void eGameWidget::sendMessage(const std::string& text) {
    mServer->sendMessage(mClientId, text);
}

bool eGameWidget::consumePotion(
    const int x, const bool merc) {
    if(merc) {
        const auto& merc = eGameWidget::merc();
        if(!merc || !merc->fUnitId) return false;
        return consumePotion(x, merc->fUnitId);
    } else {
        return consumePotion(x, mClientId);
    }
}

bool eGameWidget::consumePotion(
    const int x, const uint32_t unitId) {
    auto& eq = equipment();
    const auto p = eq.takePotion(x);
    return consumePotion(p, unitId);
}

bool eGameWidget::consumePotion(
    const eItem& p, const bool merc) {
    if(merc) {
        const auto& merc = eGameWidget::merc();
        if(!merc || !merc->fUnitId) return false;
        return consumePotion(p, merc->fUnitId);
    } else {
        return consumePotion(p, mClientId);
    }
}

bool eGameWidget::consumePotion(
    const eItem& p, const uint32_t unitId) {
    if(p.fType != eItemType::potion) return false;
    return mServer->consumePotion(mClientId, p.fItemId, unitId);
}

void eGameWidget::waypointTeleport(
    const eAreaIds& area) {
    eMoveToMapData moveData;
    moveData.fType = eMoveToMapType::waypoint;
    moveData.fTo = area;
    sMoveToMap(moveData);
}

void eGameWidget::spawnPortal() {
    mServer->spawnPortal(mClientId);
}

void eGameWidget::openSellerMenu(const uint32_t sellerId) {
    mServer->requestSeller(mClientId, sellerId);
}

void eGameWidget::addSocket(const uint8_t questId) {
    mServer->addedSocket(mClientId, questId);
}

std::string eGameWidget::name(const uint32_t uid) const {
    const auto& s = eSlayers::sSlayers;
    const auto it = s.find(uid);
    if(it == s.end()) {
        const auto& merc = eGameWidget::merc();
        if(merc && merc->fUnitId == uid) {
            const auto& names = eMercenaryNames::sNames.get(merc->fMercType);
            return names[merc->fNameId % names.size()];
        }
    } else {
        const auto& slayer = it->second;
        return slayer.fName;
    }
    return "";
}

std::string eGameWidget::name(const eUnit& u) const {
    if(u.isSlayerBody()) {
        return mCName;
    }
    const auto charId = u.fCharId;
    const auto name = eGameWidget::name(charId);
    if(name.empty()) {
        const auto typeId = u.fUnitInfoId;
        return eMonsterNames::name(typeId);
    }
    return name;
}

std::string eGameWidget::name(const std::shared_ptr<eUnit>& u) const {
    if(!u) return "";
    return name(*u);
}

bool eGameWidget::hire(const eHireInfo& info) {
    auto& eq = equipment();
    if(eq.totalGold() < info.fCost) return false;
    eq.takeGold(info.fCost);
    eMercenary merc;
    merc.fMercType = info.fMercType;
    merc.fLevel = info.fLevel;
    merc.fNameId = info.fNameId;
    return mServer->summonMerc(mClientId, merc);
}

bool eGameWidget::resurrectMerc() {
    auto& merc = eGameWidget::merc();
    if(!merc) return false;
    auto& eq = equipment();
    const auto cost = merc->cost();
    if(eq.totalGold() < cost) return false;
    eq.takeGold(cost);
    return mServer->summonMerc(mClientId, *merc);
}

void eGameWidget::updateWantsToTalk() {
    const auto& quests = eGameWidget::quests();
    auto& talkHeard = eGameWidget::talkHeard();
    const auto& eq = eGameWidget::equipment();
    talkHeard.updateWantsToTalk(quests, eq);
}

void eGameWidget::sSendEqAction(const eEquipmentAction& a) {
    sInstance->sendEqAction(a);
    sInstance->mMainAction->recalculateStats();
}

void eGameWidget::sSendBuyAction(const eBuyAction& a) {
    sInstance->sendBuyAction(a);
    sInstance->mMainAction->recalculateStats();
}

void eGameWidget::sSendSellAction(const eSellAction& a) {
    sInstance->sendSellAction(a);
    sInstance->mMainAction->recalculateStats();
}

void eGameWidget::sSendSkillLevelsChanged() {
    sInstance->sendSkillLevelsChanged();
    sInstance->mMainAction->recalculateStats();
}

void eGameWidget::sSendAttributesChanged() {
    sInstance->sendAttributesChanged();
    sInstance->mMainAction->recalculateStats();
}

void eGameWidget::sMoveToMap(const eMoveToMapData& moveData) {
    sInstance->mMoveAction(moveData);
}

void eGameWidget::sDropGold(const int gold) {
    sInstance->dropGold(gold);
}

void eGameWidget::sOpenSellerMenu(
    const uint32_t sellerId) {
    sInstance->openSellerMenu(sellerId);
}

void eGameWidget::sAddSocket(const uint8_t questId) {
    sInstance->addSocket(questId);
}

void eGameWidget::paintEvent(ePainter& p) {
    mGamePainter.clear();

    const float by = 25.f/eRunSettings::sFPS;
    mServer->increment(by);

    const auto r = renderer();

    auto& eq = eGameWidget::equipment();
    {
        const auto followers = mServer->followersUpdate(mClientId);
        if(followers) {
            auto& f = eFollowers::sFollowers;
            f.clear();
            for(const auto fid : *followers) {
                f.emplace(fid, eUnitSpecialData());
            }
        }
        const auto seller = mServer->receiveSeller();
        if(seller) {
            eGameScreen::sOpenSellerMenu(*seller);
        }
        const auto replaceItemId = mServer->receiveReplaceItemId();
        if(replaceItemId) {
            eSellerWidget::sReplaceItemId(mClientId, *replaceItemId);
        }
        const auto newUsers = mServer->receiveNewUsers();
        for(const auto& u : newUsers) {
            const auto& name = u.fName;
            auto text = eText::text(12, 0);
            text = eStringHelpers::replaceAll(text, "%1", name);
            addMessage(r, text);
        }
        const auto leftUsers = mServer->receiveLeftUsers();
        for(const auto& u : leftUsers) {
            const auto name = u.fName;
            auto text = eText::text(12, 1);
            text = eStringHelpers::replaceAll(text, "%1", name);
            addMessage(r, text);
        }
        const auto slainUsers = mServer->receiveSlainUsers();
        for(const auto& u : slainUsers) {
            const auto name = u.fName;
            auto text = eText::text(12, 2);
            text = eStringHelpers::replaceAll(text, "%1", name);
            addMessage(r, text);
        }
        const auto messages = mServer->receiveMessages();
        for(const auto& msg : messages) {
            const uint32_t clientId = msg.fClientId;
            const auto name = eSlayers::name(clientId);
            const auto text = name + ": " + msg.fMsg;
            addMessage(r, text);
        }
        const auto objs = mServer->receiveObjectStateChanges();
        for(const auto& obj : objs) {
            const auto o = mMap->object(obj.fPos, obj.fObjectId);
            if(!o) continue;
            o->fState = obj.fState;
        }
        const auto doors = mServer->receiveDoorsStateChanges();
        for(const auto& d : doors) {
            mMap->triggerDoors(d);
        }
        const auto bodiesChanged = mServer->receiveBodiesChanged();
        for(const auto& changes : bodiesChanged) {
            const auto b = eBodies::get(changes.fBodyId);
            if(!b) continue;
            const auto& items = changes.fItems;
            auto& beq = b->fEq;
            for(const auto& i : items) {
                auto item = beq.takeBodyItem(i.fItemId);
                if(item.fType == eItemType::none) continue;
                eEquipmentAction action;
                std::swap(item, action.fAddItem);
                action.fPlace = i.fPlace;
                action.fType = eEquipmentActionType::add;
                action.apply(eq, eq.fDragged);
            }
        }
        const auto bodiesToAdd = mServer->receiveBodiesCreated();
        for(const auto& body : bodiesToAdd) {
            eBodies::add(body);
        }
        const auto bodiesToRemove = mServer->receiveBodiesPickedUp();
        for(const auto bodyId : bodiesToRemove) {
            eBodies::remove(bodyId);
        }
        const auto eqActions = mServer->receiveEqActions();
        for(const auto& a : eqActions) {
            a.apply(eq, eq.fDragged);
        }
        if(!eqActions.empty()) {
            mMainAction->recalculateStats();
            eHoverWidget::sUpdateDragItem(eq.fDragged);
            updateWantsToTalk();
        }

        const auto quests = mServer->receiveQuests(mClientId);
        if(quests) {
            auto& dstQuests = eGameWidget::quests();
            dstQuests = *quests;
            eGameWidget::updateWantsToTalk();
            eQuestsWidget::checkUpdated(dstQuests);
        }
    }
    const auto& res = resolution();
    const auto worldResult = mWorld.processServerData(
        mClientId, *mServer, *mMainChar,
        *mMainAction, res, r);
    if(eInventoryWidget::sBlocked) {
        const bool r = mServer->receiveEquipment(mClientId, eq);
        if(r) {
            mMainAction->recalculateStats();
            eInventoryWidget::sBlocked = false;
            eHoverWidget::sUpdateDragItem(eq.fDragged);
        } else {
            const bool r = mServer->unblockEquipment(mClientId);
            if(r) {
                eInventoryWidget::sBlocked = false;
            }
        }
    }

    if(mPortalsVersion < ePortal::version()) {
        mPortalsVersion = ePortal::version();
        const auto mapId = mMap->id();
        std::set<uint32_t> newPortals;
        const auto addPortal = [&](
            const uint32_t objId,
            const eAreaIds& area,
            const ePointF& pos) {
            if(area.fMapId != mapId) return;
            const int n = mPortals.count(objId);
            if(n > 0) return;
            const auto old = mMap->object(pos, objId);
            if(old) return;
            newPortals.emplace(objId);
            const auto new_ = std::make_shared<eObject>();
            new_->fObjectId = objId;
            new_->fPos = pos;
            const auto typeId = eObjectsInfo::sObjects.id("portal");
            const auto& info = eObjectsInfo::sObjects.get(typeId);
            new_->fObjectType = typeId;
            new_->fWidth = info.fWidth;
            new_->fHeight = info.fHeight;
            new_->fSubtype = 0;
            mMap->addObjectIfHasPortion(new_);
        };

        for(const auto& p : ePortal::sPortals) {
            if(p.fCampPortalId > 0) {
                addPortal(p.fCampPortalId,
                          p.fCampArea,
                          p.fCampPos);
            }
            if(p.fOutdoorPortalId > 0) {
                addPortal(p.fOutdoorPortalId,
                          p.fOutdoorArea,
                          p.fOutdoorPos);
            }
        }

        for(const auto pid : mPortals) {
            const int n = newPortals.count(pid);
            if(n > 0) continue;
            mMap->removeObject(pid);
        }

        std::swap(mPortals, newPortals);
    }

    if(worldResult.fReceived) {
        if(worldResult.fHasMainCharData) {
            const auto& u = worldResult.fMainCharData;
            if(mMainChar->fHealth <= 0 && u.fHealth > 0) {
                mMainChar->fPos = u.fPos;
                mMainAction->stop();
                setPressedUnit(nullptr);
                setPressedNPC(nullptr);
                clearHighlighted();
                if(mRespawnHandler) mRespawnHandler();
            } else if(u.fBlockingActionTime > 0) {
                mMainChar->fPos = u.fPos;
            }
            mMainChar->fTeamId = u.fTeamId;
            mMainChar->fHealth = u.fHealth;
            mMainChar->fState = u.fState;
            mMainChar->fBoosts = u.fBoosts;
            {
                auto& stats = eGameWidget::stats();
                auto& attrs = eGameWidget::attributes();
                stats.fHealthF = u.fHealth;
                stats.fManaF = worldResult.fMana;
                stats.fStaminaF = worldResult.fStamina;
                const auto oldLevel = attrs.fLevel;
                const auto& attrsSrc = worldResult.fAttributes;
                if(attrsSrc) attrs = *attrsSrc;
                auto& skills = stats.fBaseSkillLevels;
                skills.fRemainingPoints = worldResult.fRemainingSkillPoints;
                const auto newLevel = attrs.fLevel;
                if(oldLevel != newLevel) {
                    const auto& eq = eGameWidget::equipment();
                    stats.calculate(attrs, eq);
                    stats.calculateAuras(eq);
                }
            }
            if(u.fHealth <= 0) {
                if(mDeathHandler) mDeathHandler();
                mMainAction->stop();
            }
            mMainChar->fMaxHealth = u.fMaxHealth;
            mMainChar->fBlockingActionTime = u.fBlockingActionTime;
            if(eAnimId::isAfter(u.fAnimId, mMainChar->fAnimId)) {
                mMainChar->fAnim = u.fAnim;
                mMainChar->fAnimId = u.fAnimId;
                mMainChar->fAnimSpeed = u.fAnimSpeed;
                mMainChar->fAngle = u.fAngle;
            }
        }

        if(worldResult.fUpdateBoostsAuras) {
            auto& stats = eGameWidget::stats();
            auto& attrs = eGameWidget::attributes();
            const auto& eq = eGameWidget::equipment();
            stats.fBoosts = worldResult.fBoosts;
            stats.fAuraBoosts = worldResult.fAuras;
            stats.calculate(attrs, eq);
            stats.calculateAuras(eq);
        }

        auto& dstMerc = merc();
        const auto& srcMerc = worldResult.fMerc;
        if(dstMerc && !srcMerc) {
            dstMerc = std::nullopt;
        } else if(!dstMerc && srcMerc) {
            dstMerc = eMercenary();
            srcMerc->apply(*dstMerc);
        } else {
            srcMerc->apply(*dstMerc);
        }

        auto& model = mMainChar->model();
        model.setAggressive(worldResult.fAggressive);
    }

    const auto& upos = mMainChar->fPos;
    const auto uipos = upos.floor();
    const int areaId = mMap->areaAt(uipos);
    if(mLastArea != areaId && areaId >= 0) {
        const bool fadeIn = mLastArea >= 0;
        mLastArea = areaId;
        const auto areaNameBase = mMap->areaName(areaId);
        const auto& area = mMap->area(areaId);
        const auto areaName = eAreaNames::name(areaNameBase);
        eMiniMap::sShowAreaName(areaName);
        const auto mapId = area.fMapId;
        const auto areaId = area.fAreaId;
        const auto& mapSett = eMapsSettings::sMaps.get(mapId);
        const auto& areaSett = mapSett.fAreas.get(areaId);
        mGamePainter.setLightness(areaSett.fLightness);
        mGamePainter.setContrast(areaSett.fContrast);

        mGamePainter.clearEffects();
        for(const auto& e : areaSett.fEffects) {
            mGamePainter.addEffect(e, fadeIn);
        }
    }

    mServer->changeState(mClientId, *mMainChar);
    mMainChar->fUpdate = 0;

    mWorld.simulateMissiles(by);
    mWorld.simulateNovas(by);
    mWorld.simulateSkillAreas(by);

    const auto& mpos = mInput.mousePos();
    if(!mMenuVisible) {
        const auto mouseTilePos = pixelToTilePos(mpos);
        const auto w = window();
        const bool shiftPressed = w->shiftPressed();
        mMainAction->increment(mInput.mousePressed(),
                               mInput.rightPressed(),
                               shiftPressed,
                               mouseTilePos,
                               by);
    }

    mFrame++;

    const int tileW = eGameWidget::tileWidth();
    const int tileH = eGameWidget::tileHeight();
    {
        const auto holder = mGamePainter.switchToBase();

        const auto& terrTypes = mMap->terrainTypes();
        const auto& objTypes = mMap->objectTypes();

        enum class eRenderElementType {
            area, item, wall, object, unit, missile
        };

        enum class eRenderOrder {
            floor,
            normal,
            overlay
        };

        struct eRenderElement {
            eRenderOrder fOrder;
            eRenderElementType fType;
            std::shared_ptr<ePositioned> fPtr;
            std::shared_ptr<eTexture> fTex;
            bool fLighting = false;
        };

        struct eWall : public ePositioned {
            int fTerrainType;
            eWallType fType;
            uint8_t fEncoded;
            uint8_t fEncodedStairs;
        };

        std::vector<eRenderElement> renderElements;

        int wallMinTX = uipos.fX - 1000;
        int wallMaxTX = uipos.fX + 1000;
        int wallMinTY = uipos.fY - 1000;
        int wallMaxTY = uipos.fY + 1000;
        std::vector<std::shared_ptr<eObject>> waypoints;
        std::vector<std::shared_ptr<eObject>> flat;
        std::set<int> handledObjects;
        const auto handleTile = [&](const eTileInfo& info) {
            const int x = info.fTX;
            const int y = info.fTY;
            const auto& iobjs = mMap->objects(x, y);
            for(const int iobj : iobjs) {
                if(handledObjects.count(iobj) > 0) continue;
                handledObjects.emplace(iobj);
                const auto& obj = mMap->object(iobj);
                const auto& objRef = *obj;
                const auto objType = objRef.fObjectType;
                const auto& object = eObjectsInfo::sObjects.get(objType);
                if(object.fType == eObjectType::waypoint) {
                    waypoints.emplace_back(obj);
                    continue;
                }
                const auto& pos = objRef.fPos;
                if(object.fBlocksLight == eBlockLightType::center) {
                    const float s = 0.5f*(objRef.fWidth + objRef.fHeight);
                    mGamePainter.addObjectShadow(
                        pos.fX, pos.fY, s);
                } else if(object.fBlocksLight == eBlockLightType::rect) {
                    mGamePainter.addRectShadow(
                        pos.fX, pos.fY, objRef.fWidth, objRef.fHeight);
                }
                if(object.fLightRadius > 0.01f) {
                    const float dx = 0.5f*objRef.fWidth;
                    const float dy = 0.5f*objRef.fHeight;
                    mGamePainter.addLight(pos.fX + dx, pos.fY + dy,
                                          object.fLightRadius);
                }
                if(object.fFlat) {
                    flat.emplace_back(obj);
                    continue;
                }
                if(object.fTexId < 0) continue;
                renderElements.emplace_back(eRenderElement{eRenderOrder::normal,
                                                           eRenderElementType::object,
                                                           std::static_pointer_cast<ePositioned>(obj)});
            }
            const auto& tile = mMap->tile(x, y);
            const auto addWall = [&](const eWallType wallType,
                                     const uint8_t encoded,
                                     const uint8_t encodedStairs) {
                const auto wall = std::make_shared<eWall>();
                wall->fPos = ePointF{x, y};
                wall->fTerrainType = tile.fTerrainType;
                wall->fType = wallType;
                wall->fEncoded = encoded;
                wall->fEncodedStairs = encodedStairs;
                switch(wallType) {
                case eWallType::topRight: {
                    if(x == uipos.fX) {
                        if(y > uipos.fY) {
                            wallMaxTY = std::min(wallMaxTY, y);
                        } else {
                            wallMinTY = std::max(wallMinTY, y);
                        }
                    }
                } break;
                case eWallType::topLeft: {
                    if(y == uipos.fY) {
                        if(x > uipos.fX) {
                            wallMaxTX = std::min(wallMaxTX, x);
                        } else {
                            wallMinTX = std::max(wallMinTX, x);
                        }
                    }
                } break;
                };

                const auto terrType = tile.fTerrainType;
                const auto& info = eTerrsTexturesData::get(terrType);
                const bool doors = eTile::doors(encoded);
                const bool open = eTile::open(encoded);
                const uint8_t type = eTile::type(encoded);
                const std::vector<eWallTexture>* types = nullptr;
                switch(wallType) {
                case eWallType::topLeft:
                    types = doors ? &info.fTLDoorsOpen :
                                &info.fTLWalls;
                    break;
                case eWallType::topRight:
                    types = doors ? &info.fTRDoorsOpen :
                                &info.fTRWalls;
                    break;
                }

                const int nTypes = types->size();
                if(nTypes <= type) return;
                const auto& wtex = (*types)[type];
                const int texId = wtex.fId;

                const auto& texs = eTerrsTextures::get(terrType);
                const auto& tex = texs.getTexture(texId);
                if(info.fWallsShadow) {
                    const float wallMin = open ? wtex.fWallMin : 0.f;
                    const float wallMax = open ? wtex.fWallMax : 1.f;
                    bool minFeatherForce = false;
                    bool maxFeatherForce = false;

                    {
                        int dx = 0;
                        int dy = 0;
                        uint8_t eTile::*wallPtr = nullptr;
                        uint8_t eTile::*otherWallPtr = nullptr;
                        switch(wallType) {
                        case eWallType::topLeft:
                            dy = 1;
                            wallPtr = &eTile::fWallTL;
                            otherWallPtr = &eTile::fWallTR;
                            break;
                        case eWallType::topRight:
                            dx = 1;
                            wallPtr = &eTile::fWallTR;
                            otherWallPtr = &eTile::fWallTL;
                            break;
                        }
                        const int xMin = x - dx;
                        const int yMin = y - dy;
                        const bool rMin = mMap->inside(xMin, yMin);
                        if(rMin) {
                            const auto& min = mMap->tile(xMin, yMin);
                            const auto minWall = min.*wallPtr;
                            if(!minWall) {
                                minFeatherForce = true;
                                const auto otherWall = tile.*otherWallPtr;
                                if(otherWall) { // top corner
                                    if(upos.fX < x && upos.fY < y) {
                                        minFeatherForce = false;
                                    }
                                } else {
                                    const int sx = x - dy;
                                    const int sy = y - dx;
                                    const bool rs = mMap->inside(sx, sy);
                                    if(rs) {
                                        const auto& sideOther = mMap->tile(sx, sy);
                                        const auto sideOtherWall = sideOther.*otherWallPtr;
                                        if(sideOtherWall) { // left / right corner
                                            switch(wallType) {
                                            case eWallType::topLeft: { // right corner
                                                if(upos.fX > x && upos.fY < y) {
                                                    minFeatherForce = false;
                                                }
                                            } break;
                                            case eWallType::topRight: { // left corner
                                                if(upos.fX < x && upos.fY > y) {
                                                    minFeatherForce = false;
                                                }
                                            } break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        const int xMax = x + dx;
                        const int yMax = y + dy;
                        const bool rMax = mMap->inside(xMax, yMax);
                        if(rMax) {
                            const auto& max = mMap->tile(xMax, yMax);
                            const auto maxWall = max.*wallPtr;
                            if(!maxWall) {
                                maxFeatherForce = true;
                                const auto otherWall = max.*otherWallPtr;
                                if(otherWall) { // left / right corner
                                    switch(wallType) {
                                    case eWallType::topLeft: { // left corner
                                        if(upos.fX < xMax && upos.fY > yMax) {
                                            maxFeatherForce = false;
                                        }
                                    } break;
                                    case eWallType::topRight: { // right corner
                                        if(upos.fX > xMax && upos.fY < yMax) {
                                            maxFeatherForce = false;
                                        }
                                    } break;
                                    }
                                } else { // bottom corner
                                    if(upos.fX > xMax && upos.fY > yMax) {
                                        maxFeatherForce = false;
                                    }
                                }
                            }
                        }
                    }

                    mGamePainter.addWallShadow(
                        x, y, wallType, wallMin, wallMax,
                        minFeatherForce, maxFeatherForce);
                }

                renderElements.emplace_back(eRenderElement{eRenderOrder::normal,
                                                           eRenderElementType::wall,
                                                           std::static_pointer_cast<ePositioned>(wall)});

            };
            if(tile.fWallTL) addWall(eWallType::topLeft, tile.fWallTL, tile.fStairsTL);
            if(tile.fWallTR) addWall(eWallType::topRight, tile.fWallTR, tile.fStairsTR);
        };

        mTileIterator.nextIteration(this);
        bool iniObjs = true;
        const auto align = eAlignment::top | eAlignment::hcenter;
        for(const auto terrType : terrTypes) {
            if(terrType == 0) continue;
            const auto& texs = eTerrsTextures::get(terrType);
            const auto drawTerr = [&](const int px, const int py,
                                      const uint8_t drawTerrType,
                                      const uint8_t drawTileType) {
                if(drawTerrType != terrType) return;
                if(drawTileType == 0) return;
                const auto& tex = texs.getTexture(drawTileType);
                mGamePainter.drawTexture(px, py, tex, align);
            };
            mTileIterator.iterate([&](const eTileInfo& info) {
                const int x = info.fTX;
                const int y = info.fTY;
                if(iniObjs) handleTile(info);
                const auto& tile = mMap->tile(x, y);
                const int px = info.fPX;
                const int py = info.fPY + tileH;
                drawTerr(px, py, tile.fTerrainType, tile.fTileType);
                for(const auto& o : tile.fOverlays) {
                    drawTerr(px, py, o.fTerrainType, o.fTileType);
                }
            });
            iniObjs = false;
        }

        if(iniObjs) {
            mTileIterator.iterate([&](const eTileInfo& info) {
                handleTile(info);
            });
            iniObjs = false;
        }

        const auto window = eWidget::window();
        const bool altPressed = window->altPressed();
        mItemNames.clear();

        const float mult = res.multiplier();
        const int margin = 100*mult;
        const int w = width();
        const int h = height();
        for(const auto& i : mWorld.groundItems()) {
            const auto& pos = i->fPos;
            const auto pixel = tilePosToPixel(pos);
            if(pixel.fX < -margin || pixel.fY < -margin ||
               pixel.fX > w + margin || pixel.fY > h + margin) continue;
            const auto ipos = pos.floor();
            const auto tile = mTileIterator.getTile(ipos.fX, ipos.fY);
            if(!tile) continue;
            renderElements.emplace_back(eRenderElement{eRenderOrder::normal,
                                                       eRenderElementType::item,
                                                       std::static_pointer_cast<ePositioned>(i)});
        }
        const auto mapId = mMap->id();
        for(const auto& u : mWorld.units()) {
            if(u->fMapId != mapId) continue;
            const auto& pos = u->fPos;
            const auto pixel = tilePosToPixel(pos);
            if(pixel.fX < -margin || pixel.fY < -margin ||
               pixel.fX > w + margin || pixel.fY > h + margin) continue;
            const auto ipos = pos.floor();
            const auto tile = mTileIterator.getTile(ipos.fX, ipos.fY);
            if(!tile) continue;
            const int animId = u->fAnim;
            const bool fleshExpl = animId == sFleshExplAnim ||
                                   animId == sFleshExplBody;
            const bool iceExpl = animId == sIceExplAnim ||
                                 animId == sIceExplBody;
            if(fleshExpl || iceExpl) {
                auto& model = u->model();
                model.incFrame(by);
                const auto missileType = fleshExpl ?
                    eMissilesInfo::sFleshId :
                    eMissilesInfo::sIceId;
                const auto& missileInfo = eMissilesInfo::sMissiles.get(missileType);
                auto& missileTex = eMissilesTextures::sMissiles.get(missileType);
                const bool body = animId == sFleshExplBody ||
                                  animId == sIceExplBody;
                const int baseId = body ?
                    missileInfo.stayAnimId() :
                    missileInfo.baseAnimId();
                const int nFrames = missileInfo.nFrames(baseId);
                bool floor = body;
                int frame = model.frame();
                if(frame >= nFrames) {
                    floor = true;
                    frame = nFrames - 1;
                }
                const auto m = std::make_shared<eExtendedMissile>();
                m->fPos = u->fPos;
                const auto& ftex = missileTex.get(baseId, 0, frame);
                const auto renderOrder = floor ? eRenderOrder::floor :
                                             eRenderOrder::normal;
                renderElements.emplace_back(eRenderElement{renderOrder,
                                                           eRenderElementType::missile,
                                                           std::static_pointer_cast<ePositioned>(m),
                                                           ftex, false});
            } else {
                const int bodyId = u->bodyAnimId();
                const bool floor = animId == bodyId;
                if(!floor) {
                    const auto uinfoId = u->fUnitInfoId;
                    const auto& uinfo = eUnitsInfo::sUnits.get(uinfoId);
                    const float l = uinfo.fLighting;
                    if(l > 0.01f) {
                        const auto& pos = u->fPos;
                        mGamePainter.addLight(pos.fX, pos.fY, l);
                    }
                }
                const auto renderOrder = floor ? eRenderOrder::floor :
                                             eRenderOrder::normal;
                renderElements.emplace_back(eRenderElement{renderOrder,
                                                           eRenderElementType::unit,
                                                           std::static_pointer_cast<ePositioned>(u)});
            }
        }
        {
            const auto& pos = mMainChar->fPos;
            const auto uinfoId = mMainChar->fUnitInfoId;
            const auto& uinfo = eUnitsInfo::sUnits.get(uinfoId);
            const float l = uinfo.fLighting;
            if(l > 0.01f) {
                mGamePainter.addLight(pos.fX, pos.fY, l);
            }
            const auto ipos = pos.floor();
            const auto tile = mTileIterator.getTile(ipos.fX, ipos.fY);
            if(tile) {
                renderElements.emplace_back(eRenderElement{eRenderOrder::normal,
                                                           eRenderElementType::unit,
                                                           std::static_pointer_cast<ePositioned>(mMainChar),
                                                           nullptr, true});
            }
        }
        for(const auto& m : mWorld.missiles()) {
            const auto& pos = m->fPos;
            const auto pixel = tilePosToPixel(pos);
            const bool outside = pixel.fX < -margin || pixel.fY < -margin ||
                                 pixel.fX > w + margin || pixel.fY > h + margin;
            const auto ipos = pos.floor();
            const auto tile = mTileIterator.getTile(ipos.fX, ipos.fY);
            if(!tile) continue;
            const auto missileType = m->fType;
            const auto& missileInfo = eMissilesInfo::sMissiles.get(missileType);
            const int appearId = missileInfo.appearAnimId();
            const int baseId = missileInfo.baseAnimId();
            const int hitId = missileInfo.hitAnimId();
            int& frame = m->fFrame;
            int& animId = m->fAnimId;
            if(outside) {
                if(animId == hitId) {
                    mWorld.removeMissile(*m);
                }
                continue;
            }
            int nFrames = missileInfo.nFrames(animId);
            if(frame >= nFrames) {
                if(animId == appearId) {
                    animId = baseId;
                    frame = 0;
                    nFrames = missileInfo.nFrames(animId);
                }

                if(animId == hitId || animId < 0) {
                    mWorld.removeMissile(*m);
                    continue;
                }
            }
            float lmult = 1.f;
            if(animId == hitId) {
                lmult = 1.5f * (1.f - static_cast<float>(frame)/nFrames);
            }
            const int dirs = missileInfo.nDirs(animId);
            const float ainc = 360.f/dirs;
            int dir = std::round(m->fAngle/ainc) + 2*dirs/16;
            dir = (dirs + dir) % dirs;
            const int texFrame = frame++ % nFrames;
            auto& missileTex = eMissilesTextures::sMissiles.get(missileType);
            const auto& ftex = missileTex.get(animId, dir, texFrame);
            const float lradius = missileInfo.lighting();
            const bool lighting = lradius > 0.01f;
            if(lighting) {
                mGamePainter.addLight(pos.fX, pos.fY, lmult*lradius);
            }
            renderElements.emplace_back(eRenderElement{eRenderOrder::normal,
                                                       eRenderElementType::missile,
                                                       std::static_pointer_cast<ePositioned>(m),
                                                       ftex, lighting});
        }
        for(auto& n : mWorld.novas()) {
            const auto missileType = n->fMissileType;
            if(missileType == 0) continue;
            const auto& c = n->fCenter;
            const float r = n->fRadius;
            const float speed = n->fSpeed;
            const float maxR = n->fMaxRadius;
            const int nMissiles = n->fNMissiles;
            const eVec2f displ{r, 0.f};
            const float mangle = 180.f;
            const float dangle = 360.f/nMissiles;
            const auto& intervals = n->fIntervals;
            const auto& missileInfo = eMissilesInfo::sMissiles.get(missileType);
            auto& missileTex = eMissilesTextures::sMissiles.get(missileType);
            const auto type = missileInfo.type();
            const int appearId = missileInfo.appearAnimId();
            const int baseId = missileInfo.baseAnimId();
            const int hitId = missileInfo.hitAnimId();
            const int hitNFrames = hitId < 0 ? 0 : missileInfo.nFrames(hitId);

            int novaFrame = n->fFrame;
            int novaAnimId = appearId;
            const int nAppearFrames = appearId < 0 ? 0 :
                missileInfo.nFrames(appearId);
            if(novaFrame >= nAppearFrames) {
                novaAnimId = baseId;
                novaFrame -= nAppearFrames;
            }

            float lmult = 1.f;
            const float dimR = hitNFrames == 0 ? 0.8f*maxR :
                (maxR - hitNFrames*speed);
            if(r > dimR) {
                lmult = 1.f - (r - dimR)/(maxR - dimR);
                if(hitNFrames > 0) {
                    novaAnimId = hitId;
                    novaFrame = hitNFrames - 1 - (r - maxR)/speed;
                }
            }
            const float light = missileInfo.lighting();
            const bool lighting = light > 0.01f;
            if(lighting) {
                mGamePainter.addLight(c.fX, c.fY, lmult*(r + light));
            }

            bool& ini = n->fInitialized;
            switch(type) {
            case eMissileType::regular: {
                if(!ini) {
                    ini = true;
                    for(int i = 0; i < nMissiles; i++) {
                        const auto m = std::make_shared<eExtendedMissile>();
                        m->fAngle = mangle + i*dangle;
                        m->fType = missileType;
                        n->fMissiles.emplace_back(m);
                    }
                }

                for(const auto& m : n->fMissiles) {
                    if(m->fHit) continue;
                    auto rdispl = displ;
                    const float angle = m->fAngle - mangle;
                    rdispl.rotate(angle);
                    const ePointF pos = c + rdispl;
                    const bool in = intervals.angleInRange(angle);
                    const auto pixel = tilePosToPixel(pos);
                    if(pixel.fX < -margin || pixel.fY < -margin ||
                        pixel.fX > w + margin || pixel.fY > h + margin) continue;
                    if(in) {
                        m->fPos = pos;
                        m->fAnimId = novaAnimId;
                        m->fFrame = novaFrame;
                    } else {
                        int& animId = m->fAnimId;
                        int& frame = m->fFrame;
                        if(animId != hitId) {
                            animId = hitId;
                            frame = 0;
                        } else {
                            frame++;
                        }
                        if(frame + 1 >= hitNFrames) {
                            m->fHit = true;
                            if(animId < 0) continue;
                        }
                    }
                    const auto ipos = pos.floor();
                    const auto tile = mTileIterator.getTile(ipos.fX, ipos.fY);
                    if(tile) {
                        const int animId = m->fAnimId;
                        const int dirs = missileInfo.nDirs(animId);
                        const float ainc = 360.f/dirs;
                        const float angle = m->fAngle;
                        int dir = std::round(angle/ainc) + 2*dirs/16;
                        dir = (dirs + dir) % dirs;
                        const int frame = m->fFrame;
                        const int nFrames = missileInfo.nFrames(animId);
                        const int texFrame = frame % nFrames;
                        const auto& ftex = missileTex.get(animId, dir, texFrame);
                        renderElements.emplace_back(eRenderElement{eRenderOrder::normal,
                                                                   eRenderElementType::missile,
                                                                   std::static_pointer_cast<ePositioned>(m),
                                                                   ftex, lighting});
                    }
                }
            } break;
            case eMissileType::explosion: {
                const int nFrames = missileInfo.nFrames(novaAnimId);
                const int frame = std::min(novaFrame, nFrames - 1);
                const auto m = std::make_shared<eExtendedMissile>();
                m->fPos = n->fCenter;
                m->fAngle = 0.f;
                m->fType = missileType;
                m->fAnimId = novaAnimId;
                m->fFrame = frame;
                const auto& ftex = missileTex.get(novaAnimId, 0, frame);
                renderElements.emplace_back(eRenderElement{eRenderOrder::normal,
                                                           eRenderElementType::missile,
                                                           std::static_pointer_cast<ePositioned>(m),
                                                           ftex, lighting});
            } break;
            default:
                break;
            }

            n->fFrame++;
        }

        auto& areas = mWorld.skillAreas();
        for(const auto& a : areas) {
            const auto id = a->fId;
            const auto missileType = a->fMissileId;
            if(missileType == 0) {
                areas.remove(a->fId);
                continue;
            }
            const auto& missileInfo = eMissilesInfo::sMissiles.get(missileType);
            auto& missileTex = eMissilesTextures::sMissiles.get(missileType);
            const int appearAnimId = missileInfo.appearAnimId();
            const int baseAnimId = missileInfo.baseAnimId();
            const int disappearAnimId = missileInfo.disappearAnimId();
            int animId = appearAnimId;
            int& frame = a->fFrame;
            auto& state = a->fState;
            switch(state) {
            case eSkillAreaState::appear: {
                int appearFrames = 0;
                if(appearAnimId >= 0) {
                    appearFrames = missileInfo.nFrames(appearAnimId);
                }
                if(frame >= appearFrames) {
                    if(baseAnimId >= 0) {
                        state = eSkillAreaState::base;
                        frame = 0;
                        animId = baseAnimId;
                    } else {
                        areas.remove(id);
                        continue;
                    }
                }
            } break;
            case eSkillAreaState::base: {
                animId = baseAnimId;
                int baseFrames = 0;
                if(baseAnimId >= 0) {
                    baseFrames = missileInfo.nFrames(baseAnimId);
                }
                if(frame >= baseFrames) {
                    if(disappearAnimId >= 0) {
                        const int disappearFrames = missileInfo.nFrames(
                            disappearAnimId);
                        if(a->fRemTime < disappearFrames) {
                            state = eSkillAreaState::disappear;
                            if(disappearFrames > 0) {
                                frame = 0;
                                animId = disappearAnimId;
                            } else {
                                areas.remove(id);
                                continue;
                            }
                        } else {
                            frame = 0;
                        }
                    } else {
                        int appearFrames = 0;
                        if(appearAnimId >= 0) {
                            appearFrames = missileInfo.nFrames(appearAnimId);
                        }
                        if(a->fRemTime < appearFrames) {
                            state = eSkillAreaState::disappear;
                            if(appearFrames > 0) {
                                frame = appearFrames - 1;
                                animId = appearAnimId;
                            } else {
                                areas.remove(id);
                                continue;
                            }
                        } else {
                            frame = 0;
                        }
                    }
                }
            } break;
            case eSkillAreaState::disappear: {
                if(disappearAnimId >= 0) {
                    const int disappearFrames = missileInfo.nFrames(
                        disappearAnimId);
                    animId = disappearAnimId;
                    if(frame >= disappearFrames) {
                        areas.remove(id);
                        continue;
                    }
                } else {
                    animId = appearAnimId;
                    if(frame <= 0) {
                        areas.remove(id);
                        continue;
                    }
                }
            } break;
            }

            const float l = missileInfo.lighting();
            const bool lighting = l > 0.01f;
            if(lighting) {
                const auto& c = a->fPos;
                const float aR = a->fRadius;
                mGamePainter.addLight(c.fX, c.fY, aR*l);
            }

            const auto& ftex = missileTex.get(animId, 0, frame);
            const auto type = missileInfo.type();
            const auto order = type == eMissileType::overlay ?
                                   eRenderOrder::overlay :
                                   eRenderOrder::floor;
            renderElements.emplace_back(eRenderElement{order,
                                                       eRenderElementType::area,
                                                       std::static_pointer_cast<ePositioned>(a),
                                                       ftex, lighting});

            switch(state) {
            case eSkillAreaState::appear:
            case eSkillAreaState::base:
                frame++;
                break;
            case eSkillAreaState::disappear: {
                if(animId == appearAnimId) {
                    frame--;
                } else {
                    frame++;
                }
            } break;
            }
        }

        std::sort(renderElements.begin(), renderElements.end(),
                  [&](const eRenderElement& e1,
                      const eRenderElement& e2) {
            const auto& u1 = e1.fPtr;
            const auto& u2 = e2.fPtr;
            if(!u1 && !u2) return false;
            if(!u1) return true;
            if(!u2) return false;

            switch(e1.fOrder) {
            case eRenderOrder::floor: {
                switch(e2.fOrder) {
                case eRenderOrder::floor:
                    break;
                case eRenderOrder::normal:
                case eRenderOrder::overlay:
                    return true;
                }
            } break;
            case eRenderOrder::normal: {
                switch(e2.fOrder) {
                case eRenderOrder::floor:
                    return false;
                case eRenderOrder::normal:
                    break;
                case eRenderOrder::overlay:
                    return true;
                }
            } break;
            case eRenderOrder::overlay: {
                switch(e2.fOrder) {
                case eRenderOrder::floor:
                case eRenderOrder::normal:
                    return false;
                case eRenderOrder::overlay:
                    break;
                }
            } break;
            }

            const auto& p1 = u1->fPos;
            const auto& p2 = u2->fPos;

            const auto ip1 = p1.floor();
            const auto ip2 = p2.floor();

            if(ip1.fY != ip2.fY) return ip1.fY < ip2.fY;
            if(ip1.fX != ip2.fX) return ip1.fX < ip2.fX;

            if(p1.fY != p2.fY) return p1.fY < p2.fY;
            if(p1.fX != p2.fX) return p1.fX < p2.fX;
            return static_cast<int>(e1.fType) < static_cast<int>(e2.fType);
        });

        clearHighlighted();
        if(const auto p = mPressedUnit.lock()) {
            if(p->fHealth <= 0) {
                setPressedUnit(nullptr);
            }
        }

        for(const auto& w : waypoints) {
            const auto& obj = *w;
            const auto objType = obj.fObjectType;
            const auto& object = eObjectsInfo::sObjects.get(objType);
            const auto texObjectId = object.fTexId;
            const auto& objectTex = eObjsTextures::get(texObjectId);
            const auto& types = objectTex.fTypes;
            auto pos = obj.fPos;
            const float width = object.fWidth;
            const float height = object.fHeight;
            pos.fX += width*0.5f;
            pos.fY += height*0.5f;
            const auto ipixel = tilePosToIPixel(pos);
            const auto& type = types[0];
            const auto& state = type[0];
            const auto& tex = state.fTexs.getTexture(0);
            mGamePainter.drawTexture(ipixel.fX, ipixel.fY,
                                     tex, eAlignment::center);
        }

        for(const auto& f : flat) {
            const auto& obj = *f;

            const auto& pos = obj.fPos;
            const auto ipixel = tilePosToIPixel(pos);

            const auto objType = obj.fObjectType;
            const auto& object = eObjectsInfo::sObjects.get(objType);
            const auto texObjectId = object.fTexId;
            const auto& objectTex = eObjsTextures::get(texObjectId);
            const auto& types = objectTex.fTypes;
            const auto typeId = obj.fSubtype % types.size();
            const auto& type = types[typeId];
            const auto& tex = type[obj.fState].fTexs.getTexture(0);
            const int h = 0.5f*(object.fWidth + object.fHeight)*tileH;
            const auto align = eAlignment::top | eAlignment::hcenter;
            mGamePainter.drawTexture(ipixel.fX, ipixel.fY + h, tex, align);
        }

        mGamePainter.calculateAndRenderLighting();

        for(const auto& w : waypoints) {
            const auto& obj = *w;
            const auto objType = obj.fObjectType;
            const auto& object = eObjectsInfo::sObjects.get(objType);
            const auto texObjectId = object.fTexId;
            const auto& objectTex = eObjsTextures::get(texObjectId);
            const auto& types = objectTex.fTypes;
            auto pos = w->fPos;
            const float width = object.fWidth;
            const float height = object.fHeight;
            pos.fX += width*0.5f;
            pos.fY += height*0.5f;
            const auto iPos = pos.floor();

            const int areaId = mMap->areaAt(iPos);
            const eAreaIds area(mapId, areaId);
            const auto& waypoints = eGameWidget::waypoints();
            const bool known = waypoints.known(area);
            auto& stateId = w->fState;
            stateId = known ? 1 : 2;

            const auto ipixel = tilePosToIPixel(pos);
            const auto& type = types[0];
            const auto& state = type[stateId];
            const auto& tex = state.fTexs.getTexture(0);
            int drawX = ipixel.fX;
            int drawY = ipixel.fY;
            const int texW = tex->width();
            const int texH = tex->height();
            ePainter::drawCoordinates(drawX, drawY, texW, texH,
                                      eAlignment::center);

            bool highlight = false;
            const SDL_Point p{int(mpos.fX), int(mpos.fY)};
            const SDL_Rect rect{drawX, drawY, texW, texH};
            const bool r = SDL_PointInRect(&p, &rect);
            if(r) {
                setHighlightedObject(w);
                mHighlightItem.reset();
                highlight = true;
            }
            eRenderCall c(eRenderCallType::object,
                          pos.fX, pos.fY,
                          drawX, drawY, tex);
            c.fHighlight = highlight;
            c.fShadow = false;
            mGamePainter.render(c);
        }

        for(const auto& e : renderElements) {
            const auto& ePtr = e.fPtr;
            if(!ePtr) continue;
            const auto& pos = ePtr->fPos;
            const auto iPos = pos.floor();
            const auto ipixel = tilePosToIPixel(pos);
            if(e.fType == eRenderElementType::unit) {
                const auto u = std::static_pointer_cast<eUnit>(ePtr);
                const auto infoId = u->fUnitInfoId;
                const auto& info = eUnitsInfo::sUnits.get(infoId);
                auto& model = u->model();
                const bool frozen = u->frozen();
                const bool cold = u->cold();
                if(!frozen) {
                    if(cold) {
                        model.incFrame(by*eUnitData::sColdSpeed);
                    } else {
                        model.incFrame(by);
                    }
                }
                bool highlight = false;
                if(!mHighlightUnit.lock() && u != mMainChar &&
                    (u->fHealth > 0 || u->isSlayerBody())) {
                    const SDL_Point p{int(mpos.fX), int(mpos.fY)};
                    const int w = 0.75*u->fRadius*tileW;
                    const int h = 2*w;
                    const SDL_Rect rect{ipixel.fX - w/2, ipixel.fY - h, w, h};
                    highlight = SDL_PointInRect(&p, &rect);
                    if(highlight) {
                        const auto b = model.requestBoundingRect();
                        const SDL_Rect rect{ipixel.fX + b.x, ipixel.fY + b.y, b.w, b.h};
                        highlight = SDL_PointInRect(&p, &rect);
                        if(highlight) {
                            clearHighlighted();

                            switch(info.fNPCType) {
                            case eNPCType::none: {
                                setHighlightedUnit(u);
                            } break;
                            default: {
                                setHighlightedNPC(u);
                            } break;
                            }
                        }
                    }
                }
                if(const auto p = mPressedUnit.lock()) {
                    highlight = p == u;
                } else if(const auto p = mPressedNPC.lock()) {
                    highlight = p == u;
                }
                SDL_FColor colorMod{1.f, 1.f, 1.f, 1.f};
                const bool poisoned = u->poisoned();
                if(cold || frozen) {
                    colorMod = SDL_FColor{1.f, 1.5f, 3.f, 1.f};
                } else if(poisoned) {
                    colorMod = SDL_FColor{0.f, 1.f, 0.2f, 1.f};
                }
                {
                    const auto& color = info.fColor;
                    colorMod.r *= color.fR;
                    colorMod.g *= color.fG;
                    colorMod.b *= color.fB;
                    colorMod.a *= color.fA;
                }
                for(const auto mod : u->fMods) {
                    const auto& info = eEliteModifiersInfo::sElite.get(mod);
                    const auto& color = info.fBossColorMod;
                    colorMod.r *= color.fR;
                    colorMod.g *= color.fG;
                    colorMod.b *= color.fB;
                    colorMod.a *= color.fA;
                }

                const auto tex = model.requestTexture(r);
                if(!tex) continue;
                const auto rect = model.requestBoundingRect();

                const auto& bs = u->fBoosts;
                if(!bs.empty()) {
                    int nCurses = 0;
                    int nAuras = 0;
                    for(const uint8_t boost : u->fBoosts) {
                        const auto& missileInfo = eMissilesInfo::sMissiles.get(boost);
                        const auto type = missileInfo.type();
                        switch(type) {
                        case eMissileType::curse:
                            nCurses++;
                            break;
                        case eMissileType::aura:
                            nAuras++;
                            break;
                        default:
                            continue;
                        }
                    }
                    const int displayFrames = 100;
                    const uint16_t frame = mFrame + 16*u->fCharId;
                    int curseId = nCurses <= 0 ? -1 :
                        (frame / displayFrames) % nCurses;
                    int auraId = nAuras <= 0 ? -1 :
                        (frame / displayFrames) % nAuras;
                    for(const uint8_t boost : u->fBoosts) {
                        const auto& missileInfo = eMissilesInfo::sMissiles.get(boost);
                        auto& missileTex = eMissilesTextures::sMissiles.get(boost);
                        const int drawX = ipixel.fX;
                        int drawY;
                        const auto type = missileInfo.type();
                        switch(type) {
                        case eMissileType::curse: {
                            if(curseId-- != 0) {
                                continue;
                            }
                            drawY = ipixel.fY - mult*100;
                        } break;
                        case eMissileType::aura: {
                            if(auraId-- != 0) {
                                continue;
                            }
                            drawY = ipixel.fY;
                        } break;
                        default:
                            continue;
                        }

                        const int baseId = missileInfo.baseAnimId();
                        const int nFrames = missileInfo.nFrames(baseId);
                        if(nFrames <= 0) continue;
                        const uint16_t frame = (mFrame + 16*u->fCharId) % nFrames;
                        const auto& ftex = missileTex.get(baseId, 0, frame);
                        const eRenderCall c(eRenderCallType::missile,
                                            pos.fX, pos.fY,
                                            drawX, drawY, ftex);
                        mGamePainter.render(c);
                    }
                }

                const int drawX = ipixel.fX + rect.x;
                const int drawY = ipixel.fY + rect.y;

                switch(info.fNPCType) {
                case eNPCType::healer:
                case eNPCType::trader:
                case eNPCType::mercenary:
                case eNPCType::wounded:
                case eNPCType::message: {
                    const auto& quests = eGameWidget::quests();
                    auto& talkHeard = eGameWidget::talkHeard();
                    const auto& eq = equipment();
                    eNPC npc;
                    npc.fType = eTalkNPCType::unit;
                    npc.fTypeId = infoId;
                    npc.fId = u->fCharId;
                    const bool w = talkHeard.wantsToTalk(
                        npc, quests, eq);
                    if(w) {
                        const int texW = tex->width();
                        const auto& bubble = eUITextures::sTalk;
                        const int x = drawX + texW/2;
                        const int bh = bubble->height();
                        const int y = drawY - bh;
                        const auto a = eAlignment::hcenter | eAlignment::top;
                        mGamePainter.drawTexture(x, y, bubble, a);
                    }
                } break;
                case eNPCType::none:
                    break;
                }
                eRenderCall c(eRenderCallType::unit,
                              pos.fX, pos.fY,
                              drawX, drawY, tex);
                c.fHighlight = highlight;
                c.fShadow = true;
                c.fLighting = e.fLighting;
                c.fColorMod = colorMod;
                mGamePainter.render(c);
            } else if(e.fType == eRenderElementType::missile) {
                const auto& ftex = e.fTex;
                eRenderCall c(eRenderCallType::missile,
                              pos.fX, pos.fY,
                              ipixel.fX, ipixel.fY,
                              ftex);
                c.fLighting = e.fLighting;
                mGamePainter.render(c);
            } else if(e.fType == eRenderElementType::area) {
                const auto a = std::static_pointer_cast<eExtendedSkillArea>(ePtr);
                const auto missileType = a->fMissileId;
                if(missileType == 0) continue;
                const auto& missileInfo = eMissilesInfo::sMissiles.get(missileType);
                const float aR = a->fRadius;
                const float texR = missileInfo.radius();
                const float scale = aR/texR;
                const auto& ftex = e.fTex;
                eRenderCall c(eRenderCallType::area,
                              pos.fX, pos.fY,
                              ipixel.fX, ipixel.fY,
                              ftex);
                c.fLighting = e.fLighting;
                c.fScale = scale;
                mGamePainter.render(c);
            } else if(e.fType == eRenderElementType::item) {
                const auto i = std::static_pointer_cast<eGroundItem>(ePtr);
                const auto dataId = i->fDataId;
                auto& itex = eItemsTextures::getByItemDataId(dataId);
                itex.request(r, res);
                const auto& tex = itex.fTinyTex;
                const auto colorMod = eItemInstanceTexture::color(*i);
                bool highlight = false;
                const int texW = tex->width();
                const int texH = tex->height();
                int drawX = ipixel.fX;
                int drawY = ipixel.fY;
                ePainter::drawCoordinates(drawX, drawY, texW, texH, eAlignment::center);
                if(!mHighlightUnit.lock() && !mHighlightObject.lock() && !mHighlightItem.lock()) {
                    const SDL_Point p{int(mpos.fX), int(mpos.fY)};
                    const SDL_Rect rect{drawX, drawY, texW, texH};
                    const bool r = SDL_PointInRect(&p, &rect);
                    if(r) {
                        clearHighlighted();
                        setHighlightedItem(i);
                        highlight = true;
                    }
                }

                eRenderCall c(eRenderCallType::item,
                              pos.fX, pos.fY,
                              drawX, drawY,
                              tex);
                c.fHighlight = highlight;
                c.fColorMod = colorMod;
                mGamePainter.render(c);
            } else if(e.fType == eRenderElementType::object) {
                const auto objPtr = std::static_pointer_cast<eObject>(ePtr);
                const auto& obj = *objPtr;
                const auto objType = obj.fObjectType;
                const auto& object = eObjectsInfo::sObjects.get(objType);
                const auto texObjectId = object.fTexId;
                const auto& objectTex = eObjsTextures::get(texObjectId);
                const auto& types = objectTex.fTypes;
                const auto typeId = obj.fSubtype % types.size();
                const auto& type = types[typeId];
                const auto& tex = type[obj.fState].fTexs.getTexture(0);
                const int h = 0.5f*(object.fWidth + object.fHeight)*tileH;

                bool highlight = false;
                const int texW = tex->width();
                const int texH = tex->height();
                const auto otype = obj.fObjectType;
                const auto& info = eObjectsInfo::sObjects.get(otype);
                int drawX = info.fSplit ? ipixel.fX - tileW/2 : ipixel.fX ;
                int drawY = ipixel.fY + h;
                const auto align = info.fSplit ?
                    eAlignment::top | eAlignment::right :
                    eAlignment::top | eAlignment::hcenter;
                ePainter::drawCoordinates(drawX, drawY, texW, texH, align);
                bool highlightable = false;
                switch(info.fType) {
                case eObjectType::treasure: {
                    highlightable = obj.fState == 0;
                } break;
                case eObjectType::waypoint:
                    break;
                case eObjectType::portal: {
                    const auto creator = ePortal::creator(obj.fObjectId);
                    const auto t1 = eTeams::playerTeam(creator);
                    const auto t2 = eTeams::playerTeam(mClientId);
                    highlightable = t1 == t2;
                } break;
                case eObjectType::message: {
                    const auto& quests = eGameWidget::quests();
                    auto& talkHeard = eGameWidget::talkHeard();
                    const auto& eq = equipment();
                    eNPC npc;
                    npc.fType = eTalkNPCType::object;
                    npc.fTypeId = objType;
                    npc.fId = obj.fObjectId;
                    const bool w = talkHeard.wantsToTalk(
                        npc, quests, eq);
                    if(w) {
                        const auto& bubble = eUITextures::sTalk;
                        const int x = drawX + texW/2;
                        const int bh = bubble->height();
                        const int y = drawY - bh;
                        const auto a = eAlignment::hcenter | eAlignment::top;
                        mGamePainter.drawTexture(x, y, bubble, a);
                    }
                }
                case eObjectType::trapDoor:
                case eObjectType::stash:
                case eObjectType::portalDoor:
                    highlightable = true;
                    break;
                default:
                    break;
                };
                if(highlightable && !mHighlightUnit.lock() && !mHighlightObject.lock()) {
                    const SDL_Point p{int(mpos.fX), int(mpos.fY)};
                    const SDL_Rect rect{drawX, drawY, texW, texH};
                    const bool r = SDL_PointInRect(&p, &rect);
                    if(r) {
                        clearHighlighted();
                        setHighlightedObject(objPtr);
                        highlight = true;
                    }
                }
                eRenderCall c(eRenderCallType::object,
                              pos.fX + obj.fWidth,
                              pos.fY + obj.fHeight,
                              drawX, drawY, tex);
                c.fHighlight = highlight;
                c.fShadow = object.fShadow;
                c.fObjSize = 0.5f*(obj.fWidth + obj.fHeight);
                c.fObjSplitLighting = object.fSplit;
                mGamePainter.render(c);
            } else if(e.fType == eRenderElementType::wall) {
                const auto& wall = static_cast<eWall&>(*ePtr);
                const auto terrType = wall.fTerrainType;
                const auto& info = eTerrsTexturesData::get(terrType);
                const uint8_t encoded = wall.fEncoded;
                const bool doors = eTile::doors(encoded);
                const bool open = eTile::open(encoded);
                const uint8_t sencoded = wall.fEncodedStairs;
                const bool stairs = eTile::stairs(sencoded);
                const uint8_t type = eTile::type(encoded);
                const bool other = eTile::other(encoded);
                const std::vector<eWallTexture>* types = nullptr;
                switch(wall.fType) {
                case eWallType::topLeft:
                    types = other ?
                        (doors ? &info.fBRDoorsOpen : &info.fBRWalls) :
                        (doors ? &info.fTLDoorsOpen : &info.fTLWalls);
                    break;
                case eWallType::topRight:
                    types = other ?
                        (doors ? &info.fBLDoorsOpen : &info.fBLWalls):
                        (doors ? &info.fTRDoorsOpen : &info.fTRWalls);
                    break;
                }

                const int nTypes = types->size();
                if(nTypes <= type) continue;
                const auto& wtex = (*types)[type];
                const int texId = wtex.fId;

                const auto& texs = eTerrsTextures::get(terrType);
                const auto& tex = texs.getTexture(texId);

                const int bottomY = ipixel.fY + tileH;

                bool transparent = false;

                if(info.fWallsTransparent) {
                    int wallMaxTX_tmp;
                    switch(wall.fType) {
                    case eWallType::topLeft:
                        wallMaxTX_tmp = wallMaxTX;
                        break;
                    case eWallType::topRight:
                        wallMaxTX_tmp = wallMaxTX - 1;
                        break;
                    }
                    const SDL_Rect transRect{wallMinTX, wallMinTY,
                                             wallMaxTX_tmp - wallMinTX + 1,
                                             wallMaxTY - wallMinTY + 1};
                    const SDL_Point pt{iPos.fX, iPos.fY};
                    if(SDL_PointInRect(&pt, &transRect)) {
                        switch(wall.fType) {
                        case eWallType::topLeft:
                            transparent = uipos.fX < iPos.fX;
                            break;
                        case eWallType::topRight:
                            transparent = uipos.fY < iPos.fY;
                            break;
                        }
                    }
                }

                const int texW = tex->width();
                const int texH = tex->height();
                int drawX = ipixel.fX;
                int drawY = bottomY;
                ePainter::drawCoordinates(drawX, drawY, texW, texH,
                                          eAlignment::top | eAlignment::hcenter);

                eRenderCall c(eRenderCallType::wall,
                              pos.fX, pos.fY,
                              drawX, drawY, tex);
                c.fWallType = wall.fType;
                c.fTransparent = transparent;
                mGamePainter.render(c);
                if(doors) {
                    bool highlight = false;
                    if(!mHighlightUnit.lock() && !mHighlightObject.lock()) {
                        eDoors doors(wall.fType, type, nTypes, iPos.fX, iPos.fY, open);
                        auto& tiles = doors.fTiles;

                        const SDL_Point p{int(mpos.fX), int(mpos.fY)};
                        const int pixelH = 2*texH/3;
                        for(const auto& tile : tiles) {
                            const auto top = tilePosToPixel(tile).round();
                            SDL_Rect hRect;
                            switch(wall.fType) {
                            case eWallType::topLeft: {
                                hRect = SDL_Rect{top.fX - tileW/2,
                                                 top.fY - pixelH,
                                                 tileW/2, pixelH};
                            } break;
                            case eWallType::topRight: {
                                hRect = SDL_Rect{top.fX,
                                                 top.fY - pixelH,
                                                 tileW/2, pixelH};
                            } break;
                            }
                            const bool h = SDL_PointInRect(&p, &hRect);
                            if(h) highlight = true;
                        }

                        if(highlight && !mHighlightDoors) {
                            clearHighlighted();
                            setHighlightedDoors(doors);
                        }
                    }

                    if(!open) {
                        const std::vector<eWallTexture>* types = nullptr;
                        switch(wall.fType) {
                        case eWallType::topLeft:
                            types = &info.fTLDoors;
                            break;
                        case eWallType::topRight:
                            types = &info.fTRDoors;
                            break;
                        }

                        const int nTypes = types->size();
                        if(nTypes <= type) continue;
                        const int texId = (*types)[type].fId;
                        const auto& tex = texs.getTexture(texId);
                        const int texW = tex->width();
                        const int texH = tex->height();
                        int drawX = ipixel.fX;
                        int drawY = bottomY;
                        ePainter::drawCoordinates(drawX, drawY, texW, texH,
                                                  eAlignment::top | eAlignment::hcenter);
                        eRenderCall c(eRenderCallType::wall,
                                      pos.fX, pos.fY,
                                      drawX, drawY,
                                      tex);
                        c.fHighlight = highlight;
                        c.fWallType = wall.fType;
                        c.fTransparent = transparent;
                        mGamePainter.render(c);
                    }
                } else if(stairs) {
                    const int stype = eTile::stairsType(sencoded);
                    const bool sup = eTile::stairsUp(sencoded);

                    const std::vector<eWallTexture>* types = nullptr;
                    switch(wall.fType) {
                    case eWallType::topLeft:
                        types = sup ? &info.fTLStairsUp :
                                      &info.fTLStairsDown;
                        break;
                    case eWallType::topRight:
                        types = sup ? &info.fTRStairsUp :
                                      &info.fTRStairsDown;
                        break;
                    }

                    const int nTypes = types->size();
                    if(nTypes <= stype) continue;

                    bool highlight = false;
                    if(!mHighlightUnit.lock() &&
                       !mHighlightObject.lock() &&
                       !mHighlightDoors) {
                        eStairs stairs(wall.fType, stype, nTypes,
                                       iPos.fX, iPos.fY, eAreaIds());
                        auto& tiles = stairs.fTiles;

                        const SDL_Point p{int(mpos.fX), int(mpos.fY)};
                        const int pixelH = 2*texH/3;
                        for(const auto& tile : tiles) {
                            const auto top = tilePosToPixel(tile).round();
                            SDL_Rect hRect;
                            switch(wall.fType) {
                            case eWallType::topLeft: {
                                hRect = SDL_Rect{top.fX - tileW/2,
                                                 top.fY - pixelH,
                                                 tileW/2, pixelH};
                            } break;
                            case eWallType::topRight: {
                                hRect = SDL_Rect{top.fX,
                                                 top.fY - pixelH,
                                                 tileW/2, pixelH};
                            } break;
                            }
                            const bool h = SDL_PointInRect(&p, &hRect);
                            if(h) highlight = true;
                        }

                        if(!tiles.empty()) {
                            const auto& tile = tiles[0];
                            const auto s = mMap->mapStairs(
                                tile.fX, tile.fY, wall.fType);
                            if(s) {
                                stairs.fTo = s->fTo;
                                if(highlight && !mHighlightStairs) {
                                    clearHighlighted();
                                    setHighlightedStairs(stairs);
                                }
                            }
                        }
                    }

                    const int texId = (*types)[stype].fId;
                    const auto& tex = texs.getTexture(texId);
                    const int texW = tex->width();
                    const int texH = tex->height();
                    int drawX = ipixel.fX;
                    int drawY = bottomY;
                    ePainter::drawCoordinates(drawX, drawY, texW, texH,
                                              eAlignment::top | eAlignment::hcenter);
                    eRenderCall c(eRenderCallType::wall,
                                  pos.fX, pos.fY,
                                  drawX, drawY,
                                  tex);
                    c.fHighlight = highlight;
                    c.fWallType = wall.fType;
                    c.fTransparent = transparent;
                    mGamePainter.render(c);
                }
            }
        }

        if(altPressed) {
            std::map<float, eGroundItem*> items;
            for(const auto& i : mWorld.groundItems()) {
                const float dist = ePointF::distance(i->fPos, mMainChar->fPos);
                items[dist] = i.get();
            }
            for(const auto& it : items) {
                const auto& i = it.second;
                const auto& pos = i->fPos;
                const auto pixel = tilePosToPixel(pos);
                const auto ipixel = pixel.floor();
                mItemNames.add(ipixel, *i);
            }
        }
        if(altPressed || mHighlightItem.lock()) {
            const SDL_Point impos{int(mpos.fX), int(mpos.fY)};
            const auto holder = mGamePainter.switchToItemNames();
            for(const auto& it : mItemNames) {
                const auto& item = it.second;
                const auto& rect = item.fRect;
                SDL_Color color;
                if(SDL_PointInRect(&impos, &rect)) {
                    color = SDL_Color{0, 0, 255, 200};
                } else {
                    color = SDL_Color{0, 0, 0, 150};
                }
                mGamePainter.fillRect(rect, color);
                mGamePainter.drawTexture(rect, item.fName,
                                         eAlignment::center);
            }
        }
    }

    mGamePainter.finish(res);

    const auto& tex = texture();
    if(tex) {
        p.drawTexture(rect(), tex, eAlignment::center);
    }

    const float mult = res.multiplier();
    const int m = res.smallPadding();
    const int x = m + 100*mult;
    int y = m;
    for(int i = 0; i < mMessages.size(); i++) {
        auto& msg = mMessages[i];
        if(msg.fFramesRemaining-- <= 0) {
            mMessages.erase(mMessages.begin() + i);
            i--;
        } else {
            p.drawTexture(x, y, msg.fTex);
            y += msg.fTex->height() + m;
        }
    }
}

bool eGameWidget::mousePressEvent(const eMouseEvent& e) {
    const auto button = e.button();
    const bool leftPressed = static_cast<bool>(
        button & eMouseButton::left);
    const bool rightPressed = static_cast<bool>(
        button & eMouseButton::right);
    if(leftPressed || rightPressed) {
        mMainAction->mousePress();
        if(e.altPreseed()) {
            uint32_t itemId;
            const bool r = mItemNames.at({e.x(), e.y()}, itemId);
            if(r) {
                const auto item = mWorld.getItem(itemId);
                if(item) {
                    mMainAction->setPressedItem(item);
                }
            }
        } else if(const auto h = mHighlightUnit.lock()) {
            setPressedUnit(h);
        } else if(const auto h = mHighlightNPC.lock()) {
            setPressedNPC(h);
        } else if(const auto o = mHighlightObject.lock()) {
            mMainAction->setPressedObject(o);
        } else if(const auto i = mHighlightItem.lock()) {
            mMainAction->setPressedItem(i);
        } else if(const auto d = mHighlightDoors) {
            mMainAction->setPressedDoors(d);
        } else if(const auto s = mHighlightStairs) {
            mMainAction->setPressedStairs(s);
        }
        mInput.handleMousePress(leftPressed, rightPressed,
                                float(e.x()), float(e.y()));
    }
    return true;
}

bool eGameWidget::mouseReleaseEvent(const eMouseEvent& e) {
    const auto button = e.button();
    const bool leftReleased = static_cast<bool>(
        button & eMouseButton::left);
    const bool rightRelease = static_cast<bool>(
        button & eMouseButton::right);
    if(leftReleased || rightRelease) {
        mInput.handleMouseRelease(leftReleased, rightRelease);
        const auto schoice = leftReleased ? eSkillChoice::left :
                                 eSkillChoice::right;
        const bool rangeAttack = mMainAction->rangedAttack(schoice);
        if(e.shiftPressed() || (rightRelease && rangeAttack) ||
           (rangeAttack && mPressedUnit.lock())) {
            mMainAction->stop();
        } else {
            const auto pos = pixelToTilePos(mInput.mousePos());
            mMainAction->mouseRelease(pos);
        }
        setPressedUnit(nullptr);
        setPressedNPC(nullptr);
    }
    return true;
}

bool eGameWidget::mouseMoveEvent(const eMouseEvent& e) {
    mInput.handleMouseMove(float(e.x()), float(e.y()));
    return true;
}

void eGameWidget::setIndicatorUnit(
    const std::shared_ptr<eUnit>& u) {
    const auto name = eGameWidget::name(u);
    mUnitIndicator->setUnit(u, name);
}

void eGameWidget::initializeTextures() {
    const int w = width();
    const int h = height();
    const int tw = mInput.tileWidth();
    const int th = mInput.tileHeight();
    const auto centerX = mInput.characterHorizontalPosPtr();
    const auto centerY = mInput.characterVerticalPosPtr();
    const auto tex = mGamePainter.initialize(
        w, h, tw, th, centerX, centerY);
    setTexture(tex);
}

void eGameWidget::setHighlightedUnit(
    const std::shared_ptr<eUnit>& u) {
    mHighlightUnit = u;
    if(mUnitIndicator && !mPressedUnit.lock()) {
        setIndicatorUnit(u);
    }
}

void eGameWidget::setHighlightedNPC(
    const std::shared_ptr<eUnit>& u) {
    mHighlightNPC = u;
    if(u) {
        const auto infoId = u->fUnitInfoId;
        const auto name = eMonsterNames::name(infoId);
        std::vector<std::string> lines;
        const auto& pos = u->fPos;
        lines.emplace_back(name);

        const eVec2f d{1.25f, 1.25f};
        const auto pixel = tilePosToPixel(pos - d);
        const auto ipixel = pixel.floor();
        const SDL_Rect rect{ipixel.fX, ipixel.fY, 0, 0};
        eHoverWidget::sSetGameTooltip(lines, rect);
    } else {
        eHoverWidget::sSetGameTooltip("");
    }
}

const std::string& areaName(const eAreaIds& area) {
    const auto mapId = area.fMapId;
    const auto areaId = area.fAreaId;
    const auto& map = eMapsSettings::sMaps.get(mapId);
    const auto areaBaseName = map.fAreas.name(areaId);
    const auto& areaName = eAreaNames::name(areaBaseName);
    return areaName;
}

void eGameWidget::setHighlightedObject(
    const std::shared_ptr<eObject>& obj) {
    mHighlightObject = obj;
    if(obj) {
        const auto type = obj->fObjectType;
        const auto name = eObjectNames::name(type);
        std::vector<std::string> lines;
        const auto& pos = obj->fPos;

        const auto objType = obj->fObjectType;
        const auto& object = eObjectsInfo::sObjects.get(objType);
        if(object.fType == eObjectType::waypoint) {
            const auto mapId = mMap->id();
            const auto areaId = mMap->areaAt(pos);
            const eAreaIds area(mapId, areaId);
            const auto& name = areaName(area);
            lines.emplace_back(name);
        } else if(object.fType == eObjectType::trapDoor) {
            if(obj->fState != 0) {
                const auto& area = obj->fTo;
                const auto& name = areaName(area);
                lines.emplace_back(name);
            }
        } else if(object.fType == eObjectType::portalDoor) {
            const auto& area = obj->fTo;
            const auto& name = areaName(area);
            lines.emplace_back(name);
        } else if(object.fType == eObjectType::portal) {
            const auto p = ePortal::portal(obj->fObjectId);
            if(p) {
                const bool toCamp = obj->fObjectId == p->fOutdoorPortalId;
                const auto& area = toCamp ? p->fCampArea : p->fOutdoorArea;
                const auto name = areaName(area);
                lines.emplace_back(name);
                const auto clientId = p->fCreator;
                const auto slayerName = eSlayers::name(clientId);
                lines.emplace_back(slayerName);
            }
        }
        lines.emplace_back(name);

        const float width = obj->fWidth;
        const float height = obj->fHeight;
        const eVec2f d{width, height};
        const auto pixel = tilePosToPixel(pos - d);
        const auto ipixel = pixel.floor();
        const SDL_Rect rect{ipixel.fX, ipixel.fY, 0, 0};
        eHoverWidget::sSetGameTooltip(lines, rect);
    } else {
        eHoverWidget::sSetGameTooltip("");
    }
}

void eGameWidget::setHighlightedDoors(
    const std::optional<eDoors>& doors) {
    mHighlightDoors = doors;
    if(doors != std::nullopt) {
        const auto pos = doors->pos();
        const auto pixel = tilePosToPixel(pos);
        const auto ipixel = pixel.floor();
        const auto& res = resolution();
        const float mult = res.multiplier();
        const int h = 100*mult;
        const SDL_Rect rect{ipixel.fX, ipixel.fY - h, 0, 0};
        const int s = doors->fOpen ? 1 : 0;
        const auto text = eText::text(15, s);
        eHoverWidget::sSetGameTooltip(text, rect);
    } else {
        eHoverWidget::sSetGameTooltip("");
    }
}

void eGameWidget::setHighlightedStairs(
    const std::optional<eStairs>& stairs) {
    mHighlightStairs = stairs;
    if(stairs != std::nullopt) {
        const auto pos = stairs->pos();
        const auto pixel = tilePosToPixel(pos);
        const auto ipixel = pixel.floor();
        const auto& res = resolution();
        const float mult = res.multiplier();
        const int h = 100*mult;
        const SDL_Rect rect{ipixel.fX, ipixel.fY - h, 0, 0};
        const auto& name = areaName(stairs->fTo);
        eHoverWidget::sSetGameTooltip(name, rect);
    } else {
        eHoverWidget::sSetGameTooltip("");
    }
}

void eGameWidget::setHighlightedItem(
    const std::shared_ptr<eGroundItem>& i) {
    mHighlightItem = i;
    if(i) {
        const auto& pos = i->fPos;
        const auto pixel = tilePosToPixel(pos);
        const auto ipixel = pixel.floor();
        mItemNames.add(ipixel, *i);
    }
}

void eGameWidget::setPressedUnit(
    const std::shared_ptr<eUnit>& u) {
    mPressedUnit = u;
    if(mUnitIndicator) {
        if(u) {
            setIndicatorUnit(u);
        } else {
            setIndicatorUnit(mHighlightUnit.lock());
        }
    }

    if(u) mMainAction->setPressedUnit(u);
}

void eGameWidget::setPressedNPC(
    const std::shared_ptr<eUnit>& u) {
    mPressedNPC = u;
    if(u) mMainAction->setPressedNPC(u);
}

void eGameWidget::clearHighlighted() {
    mHighlightNPC.reset();
    setHighlightedUnit(nullptr);
    mHighlightObject.reset();
    mHighlightItem.reset();
    mHighlightDoors.reset();
    mHighlightStairs.reset();
    eHoverWidget::sSetGameTooltip("");
}

void eGameWidget::addMessage(SDL_Renderer* const r,
                             const std::string& text) {
    const auto& res = resolution();
    const auto font = eFonts::textFont(res.tinyFontSize());
    const int w = width()/2;
    sMessageLog.emplace_back(text);
    auto& msg = mMessages.emplace_back();
    msg.fText = text;
    msg.fFramesRemaining = 5*text.size() + 250;
    eTextGenerator gen(r, eFontColor::white, font, 1, w);
    msg.fTex = gen.generate(text);
}
