#include "egamewidget.h"

#include "eskillbutton.h"

#include "eunitindicator.h"
#include "ehoverwidget.h"
#include "einventorywidget.h"
#include "../../emainwindow.h"
#include "../../screens/egamescreen.h"

#include "../../textures/eobjstextures.h"
#include "../../textures/emissilestextures.h"
#include "../../textures/etextgenerator.h"
#include "../../textures/eiteminstancetexture.h"
#include "../../textures/eitemstextures.h"

#include "../../audio/esounds.h"
#include "../../audio/esoundplayer.h"

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
                            clientId, teamId, c);
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
        const auto stats = merc->stats();
        const bool met = stats.itemReqsMet(dragged);
        if(!met) return false;;
        for(const auto type : eqO.fEquipment) {
            eEquipmentPlace p;
            auto& meq = merc->fEq;
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
    const auto p = eq.potionAt(x);
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
    if(mMainChar->fHealth <= 0) return false;
    const int soundId = eSounds::sSounds.id("swallow");
    if(soundId >= 0) eSoundPlayer::playSound(soundId);
    const bool r = mServer->consumePotion(mClientId, p.fItemId, unitId);
    if(!r) return false;
    auto& eq = equipment();
    const int x = eq.beltX(p.fItemId);
    eq.takePotion(x);
    return true;
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

void eGameWidget::sResetSkillStats(const uint8_t npcId) {
    sInstance->resetSkillStats(npcId);
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
    const auto areaId = area.fAreaId;
    const auto areaBaseName = eMapsSettings::sAreas.name(areaId);
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

void eGameWidget::resetSkillStats(const uint8_t npcId) {
    mServer->resetSkillStats(mClientId, npcId);
    auto& stats = eGameWidget::stats();
    stats.resetSkillStats();
    mMainAction->recalculateStats();
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
