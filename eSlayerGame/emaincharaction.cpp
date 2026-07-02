#include "emaincharaction.h"

#include "textures/echarstextures.h"
#include "textures/echartextures.h"
#include "units/eunit.h"

#include "widgets/gameScreen/einventorywidget.h"
#include "widgets/gameScreen/ehoverwidget.h"
#include "widgets/gameScreen/egamewidget.h"
#include "screens/egamescreen.h"

#include "names/eobjectnames.h"
#include "names/etalktext.h"

#include "etext.h"
#include "../eSlayerHelpers/include/eSlayerHelpers/etalkheard.h"

#include <eSlayerServer/eserver.h>

#include <eSlayerMapGenerator/emap.h>

#include <eSlayerHelpers/emapsettings.h>
#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/eattackdata.h>
#include <eSlayerHelpers/eunitsinfo.h>
#include <eSlayerHelpers/echardatainfo.h>
#include <eSlayerHelpers/eobjectsinfo.h>
#include <eSlayerHelpers/ewaypoints.h>
#include <eSlayerHelpers/eportals.h>
#include <eSlayerHelpers/etalk.h>

eMainCharAction::eMainCharAction(
    ePathFinderMap& map) :
    mMainChar(std::make_shared<eUnit>()),
    mMovementHandler(*mMainChar, map) {}

void eMainCharAction::initialize(const std::shared_ptr<eServer>& s,
                                 const eResolution& res,
                                 SDL_Renderer* const r,
                                 const std::shared_ptr<eMap>& map,
                                 const eOtherIterator& iter,
                                 const uint32_t clientId,
                                 const eTeamId teamId) {
    mClientId = clientId;
    mServer = s;
    mMap = map;
    const auto wPos = [this](const ePointF& pos) {
        return mMap->walkable(pos);
    };
    const auto wPath = [this](const ePointF& from,
                              const ePointF& to) {
        return mMap->walkable(from, to);
    };
    mMovementHandler.intialize(wPos, wPath, iter, clientId, teamId);
    mMovementHandler.setMoveRandom(0.f);

    const std::map<std::string, std::string> partsMap {
        {"whole", "light"}
    };
    const int typeId = 0;
    const auto& udata = eUnitsInfo::sUnits.get(typeId);
    const auto& data = eCharDataInfo::get(udata.fCharData);
    mMainCharTexs = &eCharsTextures::get(typeId);
    const float radius = udata.fRadius;
    const auto modelParts = mMainCharTexs->mapToModelParts(partsMap);

    mRunAnimId = data.animId("run");
    mWalkAnimId = data.animId("walk");
    mWalkReadyAnimId = data.animId("walkReady");
    mStandAnimId = data.animId("stand");
    mStandReadyAnimId = data.animId("standReady");

    const auto model = mMainCharTexs->requestModel(
        modelParts, res, r);
    eCharUnitModel umodel;
    umodel.setCharModel(model);
    const int animId = eMovementHandler::sChooseAnim(
        mStandAnimId, mStandReadyAnimId, false);
    umodel.setAnimation(animId, 1.f);
    umodel.setDirection(0);

    // const auto dir = "/home/ailuropoda/.eSlayer/tmp/preview/";
    // for(const auto& entry : std::filesystem::directory_iterator(dir))
    //     std::filesystem::remove_all(entry.path());
    // umodel.generatePreview(r);

    mMainChar->setModel(umodel);
    mMainChar->fRadius = radius;
    mMainChar->fUnitInfoId = typeId;
    mMainChar->fModelParts = modelParts;
    mMainChar->fTeamId = teamId;

    mStats.fSkills.emplace_back();
    mStats.fSkills.emplace_back();
}

void eMainCharAction::setPressedUnit(
    const std::shared_ptr<eUnit>& u) {
    if(u) {
        mPressedItem.reset();
        mPressedObject.reset();
        mPressedDoors.reset();
        mPressedStairs.reset();
    }
    mPressedUnit = u;
}

void eMainCharAction::setPressedItem(
    const std::shared_ptr<eGroundItem>& i) {
    if(i) {
        mPressedUnit.reset();
        mPressedObject.reset();
        mPressedDoors.reset();
        mPressedStairs.reset();
    }
    mPressedItem = i;
}

void eMainCharAction::setPressedObject(
    const std::shared_ptr<eObject>& o) {
    if(o) {
        mPressedItem.reset();
        mPressedUnit.reset();
        mPressedDoors.reset();
        mPressedStairs.reset();
    }
    mPressedObject = o;
}

void eMainCharAction::setPressedDoors(
    const std::optional<eDoors>& d) {
    if(d) {
        mPressedItem.reset();
        mPressedUnit.reset();
        mPressedObject.reset();
        mPressedStairs.reset();
    }
    mPressedDoors = d;
}

void eMainCharAction::setPressedStairs(
    const std::optional<eStairs>& s) {
    if(s) {
        mPressedItem.reset();
        mPressedUnit.reset();
        mPressedObject.reset();
        mPressedDoors.reset();
    }
    mPressedStairs = s;
}

void eMainCharAction::increment(const bool mousePressed,
                                const bool rightPressed,
                                const bool shiftPressed,
                                const ePointF& mousePos,
                                const float by) {
    if(mClickAction) return;

    if(mMainChar->fHealth <= 0) return;

    float scaledBy = by;

    if(mMainChar->frozen() || mMainChar->cold()) {
        scaledBy *= eUnitData::sColdSpeed;
    }

    auto& model = mMainChar->model();

    const eSkillChoice schoice{rightPressed ?
                       eSkillChoice::right :
                       eSkillChoice::left};

    handleAttackStop(mousePressed, rightPressed, shiftPressed);

    if(consumeActionTime(scaledBy, model)) return;

    const bool canUseSkill = mStats.canUseSkill(schoice);
    const bool rangeAttack = mStats.rangedAttack(schoice);

    ePointF targetPos = mousePos;
    bool shouldStopAttack = !canUseSkill;

    const auto& charPos = mMainChar->fPos;

    if(!shouldStopAttack) {
        if(const auto u = mPressedUnit.lock()) {
            const auto& upos = u->fPos;
            targetPos = upos;
            if(u->isSlayerBody()) {
                const float dist = ePointF::distance(upos, charPos);
                if(dist < 0.5f) {
                    const auto bodyId = u->fCharId;
                    mServer->pickupBody(mClientId, bodyId);
                    eInventoryWidget::sBlocked = true;
                    stop();
                }
            } else {
                const bool attacked = handleUnitAttack(*u, schoice, model);
                if(attacked && !mousePressed) {
                    stop();
                } else if(!attacked) {
                    // Target is out of range (e.g. knocked back),
                    // clear any active attack so we walk toward it
                    if(mAttackData.fType != eAttackTargetType::none) {
                        mAttackData = eAttackData();
                        mServer->stopAttack(mClientId);
                    }
                }
            }
        } else if(mousePressed && (shiftPressed || (rightPressed && rangeAttack))) {
            shouldStopAttack = !handlePositionAttack(mousePos, schoice, model);
        }
    }

    if(const auto item = mPressedItem.lock()) {
        const auto itemId = item->fItemId;
        const auto& ipos = item->fPos;
        const float dist = ePointF::distance(ipos, charPos);
        if(!eInventoryWidget::sBlocked && dist < 0.5f) {
            const bool dragEnabled = eInventoryWidget::sInstance;
            mServer->pickupItem(mClientId, itemId, dragEnabled);
            eInventoryWidget::sBlocked = true;
            mClickAction = mousePressed;
            stop();
            return;
        } else {
            targetPos = ipos;
        }
    } else if(const auto object = mPressedObject.lock()) {
        const auto type = object->fObjectType;
        const auto& info = eObjectsInfo::sObjects.get(type);
        const auto& opos = object->fPos;
        const float w = info.fWidth;
        const float h = info.fHeight;
        const ePointF t{opos};
        const ePointF tr{opos.fX + 0.5f*w, opos.fY};
        const ePointF r{opos.fX + w, opos.fY};
        const ePointF br{opos.fX + w, opos.fY + 0.5f*h};
        const ePointF b{opos.fX + w, opos.fY + h};
        const ePointF bl{opos.fX + 0.5f*w, opos.fY + h};
        const ePointF l{opos.fX, opos.fY + h};
        const ePointF tl{opos.fX, opos.fY + 0.5f*h};
        ePointF closesPos = t;
        float minDist = ePointF::distance(t, charPos);
        const auto handlePos = [&](const ePointF& pos) {
            const float dist = ePointF::distance(pos, charPos);
            if(dist < minDist) {
                closesPos = pos;
                minDist = dist;
            }
        };
        handlePos(tr);
        handlePos(r);
        handlePos(br);
        handlePos(b);
        handlePos(bl);
        handlePos(l);
        handlePos(tl);
        if(minDist < 0.5f) {
            const auto mapId = mMap->id();
            const auto areaId = mMap->areaAt(opos);
            const eAreaIds current(mapId, areaId);
            if(info.fType == eObjectType::waypoint) {
                eWaypoint::setKnown(current);
                const auto actId = mMap->actId();
                eGameScreen::sOpenWaypointMenu(actId, current);
            } else if(info.fType == eObjectType::stash) {
                eGameScreen::sOpenStash();
            } else if(info.fType == eObjectType::healer ||
                      info.fType == eObjectType::trader) {
                const auto baseName = eObjectsInfo::sObjects.name(type);
                const auto name = eObjectNames::name(type);

                const float width = 2*object->fWidth;
                const float height = 2*object->fHeight;
                const eVec2f d{width, height};
                const auto gw = eGameWidget::sInstance;
                const auto pixel = gw->tilePosToPixel(opos - d);
                const auto ipixel = pixel.floor();
                const SDL_Rect rect{ipixel.fX, ipixel.fY, 0, 0};

                const auto sellerId = object->fObjectId;
                if(!tryOpenTalk(sellerId, baseName, name, rect)) {
                    openMainMenu(sellerId, baseName, name, rect);
                }
            } else if(info.fType == eObjectType::trapDoor) {
                if(object->fState == 0) {
                    const eServerObject sobject(mapId, *object);
                    mServer->triggerObject(mClientId, sobject);
                } else {
                    eMoveToMapData moveData;
                    moveData.fType = eMoveToMapType::entrance;
                    moveData.fFrom = current;
                    moveData.fTo = object->fTo;
                    eGameWidget::sMoveToMap(moveData);
                }
            } else if(info.fType == eObjectType::portalDoor) {
                eMoveToMapData moveData;
                moveData.fType = eMoveToMapType::entrance;
                moveData.fFrom = current;
                moveData.fTo = object->fTo;
                eGameWidget::sMoveToMap(moveData);
            } else if(info.fType == eObjectType::portal) {
                const auto portalId = object->fObjectId;
                const auto p = ePortal::portal(portalId);
                if(p) {
                    const bool camp = p->fCampPortalId == portalId;
                    eMoveToMapData moveData;
                    moveData.fType = eMoveToMapType::portal;
                    moveData.fTo = camp ? p->fOutdoorArea :
                        p->fCampArea;
                    moveData.fPortalId = portalId;
                    eGameWidget::sMoveToMap(moveData);
                }
            } else {
                const eServerObject sobject(mapId, *object);
                mServer->triggerObject(mClientId, sobject);
            }
            mClickAction = mousePressed;
            stop();
            return;
        } else {
            targetPos = closesPos;
        }
    } else if(mPressedStairs || mPressedDoors) {
        using eDS = eDoorsStairsBase;
        const eDS& d = mPressedStairs ? static_cast<eDS&>(*mPressedStairs) :
                                        static_cast<eDS&>(*mPressedDoors);
        const auto pos = d.pos();

        const float dist = ePointF::distance(pos, charPos);
        if(dist < 0.5f) {
            const auto mapId = mMap->id();
            const auto areaId = mMap->areaAt(pos);
            if(mPressedDoors) {
                const eServerDoors sdoors(mapId, *mPressedDoors);
                mServer->triggerDoors(mClientId, sdoors);
            } else if(mPressedStairs) {
                eMoveToMapData moveData;
                moveData.fType = eMoveToMapType::entrance;
                moveData.fFrom = eAreaIds(mapId, areaId);
                moveData.fTo = mPressedStairs->fTo;
                eGameWidget::sMoveToMap(moveData);
            }
            mClickAction = mousePressed;
            stop();
            return;
        } else {
            targetPos = pos;
        }
    }

    if(shouldStopAttack && mAttackData.fType != eAttackTargetType::none) {
        stopAttack();
    }

    if(mAttackData.fType != eAttackTargetType::none) return;

    const bool hasPressedUnit = !mPressedUnit.expired();
    handleMovement(mousePressed || hasPressedUnit, targetPos, scaledBy, model);

    const float angle = mMainChar->fAngle;
    const float margin = 25.f;
    eDoors doors;
    const auto assignDoors = [&](const eWallType wall,
                                 const int x0, const int y0) {
        const auto& tile = mMap->tile(x0, y0);
        const uint8_t encoded = wall == eWallType::topLeft ?
            tile.fWallTL : tile.fWallTR;
        if(!encoded) return;
        const bool r = eTile::doors(encoded);
        if(!r) return;

        const auto terrType = tile.fTerrainType;
        const auto& info = eTerrsTexturesData::get(terrType);
        const std::vector<eWallTexture>* types = nullptr;
        float dist;
        switch(wall) {
        case eWallType::topLeft:
            dist = charPos.fX - x0;
            types = &info.fTLDoorsOpen;
            break;
        case eWallType::topRight:
            dist = charPos.fY - y0;
            types = &info.fTRDoorsOpen;
            break;
        }
        if(dist > info.fWallsThickness + 0.2f) return;
        const int nTypes = types->size();

        const uint8_t type = eTile::type(encoded);
        const bool open = eTile::open(encoded);
        doors = eDoors(wall, type, nTypes, x0, y0, open);
    };

    if(angle < 270.f + margin && angle > 270.f - margin) {
        assignDoors(eWallType::topRight, charPos.fX, charPos.fY);
    } else if(angle < 180.f + margin && angle > 180.f - margin) {
        assignDoors(eWallType::topLeft, charPos.fX, charPos.fY);
    } else if(angle < 90.f + margin && angle > 90.f - margin) {
        assignDoors(eWallType::topRight, charPos.fX, charPos.fY + 1);
    } else if(angle < margin || angle > 360.f - margin) {
        assignDoors(eWallType::topLeft, charPos.fX + 1, charPos.fY);
    } else {
        return;
    }

    const auto& tiles = doors.fTiles;
    if(tiles.empty() || doors.fOpen) {
        return;
    }
    if(mMovementHandler.pushTime() > 3.f) {
        const auto mapId = mMap->id();
        const eServerDoors sdoors(mapId, doors);
        mServer->triggerDoors(mClientId, sdoors);
    }
}

void eMainCharAction::mousePress() {
    stop();
}

void eMainCharAction::handleAttackStop(
    const bool mousePressed,
    const bool rightPressed,
    const bool shiftPressed) {
    const auto atype = mAttackData.fType;

    const bool stop =
        (atype == eAttackTargetType::character && mPressedUnit.expired()) ||
        (atype == eAttackTargetType::position && (!mousePressed || (!shiftPressed && !rightPressed)));

    if(stop) {
        stopAttack();
    }
}

bool eMainCharAction::consumeActionTime(
    const float by,
    eCharUnitModel& model) {
    const float prev = mMainChar->fBlockingActionTime;
    mMainChar->fBlockingActionTime -= by;

    if(prev > 0.f) {
        const auto anim = mMainChar->fAnim;
        const float speed = mMainChar->fAnimSpeed;
        model.setAnimation(anim, speed);
        return true;
    }
    return false;
}

bool eMainCharAction::handleUnitAttack(
    eUnit& u,
    const eSkillChoice schoice,
    eCharUnitModel& model) {
    const bool rangedAttack = mStats.rangedAttack(schoice);
    if(!rangedAttack) {
        const bool obstacle = mMap->obstacle(mMainChar->fPos, u.fPos);
        if(obstacle) return false;

        const float attackDist = mStats.attackRange(
            schoice, u.fRadius, mMainChar->fRadius);

        const float dist = ePointF::distance(mMainChar->fPos, u.fPos);
        if(dist >= attackDist) {
            return false;
        }
    }

    if(mAttackData.fType == eAttackTargetType::none) {
        const int targetId = u.fCharId;
        mAttackData = eAttackData(targetId, schoice);

        const auto vec = ePointF::vector(u.fPos, mMainChar->fPos);
        const float angle = vec.angle();
        mMainChar->fAngle = angle;
        model.setAngle(angle);

        mServer->attack(mClientId, mAttackData);
    }

    return true;
}

bool eMainCharAction::handlePositionAttack(
    const ePointF& mousePos,
    const eSkillChoice schoice,
    eCharUnitModel& model) {
    if(mAttackData.fType == eAttackTargetType::position &&
       ePointF::distance(mousePos, mAttackData.fPos) <= 0.1f) {
        return true;
    }

    mAttackData = eAttackData(mousePos, schoice);

    const auto vec = ePointF::vector(mousePos, mMainChar->fPos);
    const float angle = vec.angle();
    mMainChar->fAngle = angle;
    model.setAngle(angle);

    stand();
    mServer->attack(mClientId, mAttackData);
    return true;
}

void eMainCharAction::handleMovement(
    const bool mousePressed,
    const ePointF& pos,
    const float by,
    eCharUnitModel& model) {
    const bool run = shouldRun();
    mContinueRunning = false;
    updateWalkRunSpeed();

    bool moved = false;

    if(mousePressed) {
        mMovementHandler.moveInDirection(pos);
        moved = mMovementHandler.increment(by);
    }

    if(!moved) {
        if(mousePressed) mMovementHandler.moveTo({pos});
        moved = mMovementHandler.increment(by);
    }

    if(moved) {
        eGameScreen::sCloseObjectMenu();
    }

    updateMovementAnimation(moved, run, by, model);
}

void eMainCharAction::updateMovementAnimation(
    const bool moved,
    const bool run,
    const float by,
    eCharUnitModel& model) {
    const bool aggressive = model.aggressive();
    int animId;

    float speed = 1.f;

    if(moved) {
        const float angle = mMovementHandler.angle();
        speed = 0.5f*(1.f + mStats.fWalkRun);
        model.setAngle(angle);

        if(run) {
            mContinueRunning = true;
            animId = mRunAnimId;
        } else {
            animId = eMovementHandler::sChooseAnim(
                mWalkAnimId, mWalkReadyAnimId, aggressive);
        }
    } else {
        animId = eMovementHandler::sChooseAnim(
            mStandAnimId, mStandReadyAnimId, aggressive);
    }

    if(mMainChar->fAnim != animId) {
        mMainChar->incAnimId(1);
    }

    mMainChar->setAnim(animId);
    mMainChar->setAnimSpeed(speed);
    model.setAnimation(animId, speed);
}

void eMainCharAction::stopAttack() {
    mPressedUnit.reset();
    mAttackData = eAttackData();
    mServer->stopAttack(mClientId);
}

void eMainCharAction::recalculateStats() {
    mStats.calculate(mAttributes, mEquipment);
    mStats.calculateAuras(mEquipment);
    updateWalkRunSpeed();
}

void eMainCharAction::openMainMenu(
    const uint32_t sellerId,
    const std::string& baseName,
    const std::string& name,
    const SDL_Rect& rect) {
    const auto actions = std::make_shared<std::vector<eHoverAction>>();
    auto& actionsRef = *actions;

    const auto openMainMenu = [name, rect, actions]() {
        auto& actionsRef = *actions;
        eHoverWidget::sOpenMenu(name, actionsRef, rect);
    };

    {
        auto& talkAct = actionsRef.emplace_back();
        talkAct.fText = eText::text(20, 0);
        talkAct.fPress = [this, rect, openMainMenu, baseName]() {
            std::vector<eHoverAction> talkActions;
            const int id = eTalks::sTalk.id(baseName);
            if(id >= 0) {
                const auto relevant = mTalkHeard.allRelevant(
                    baseName, mQuests);
                for(const auto& cid : relevant) {
                    const auto& c = eTalks::get(cid);
                    const auto& title = eTalkText::title(c.fName);
                    const auto& text = eTalkText::text(c.fName);
                    auto& a = talkActions.emplace_back();
                    a.fText = title;
                    a.fPress = [rect, text]() {
                        eHoverWidget::sOpenTalk(text, nullptr, rect);
                    };
                }
            }
            {
                auto& cancelAct = talkActions.emplace_back();
                cancelAct.fText = eText::text(20, 2);
                cancelAct.fPress = openMainMenu;
            }
            eHoverWidget::sOpenMenu(eText::text(20, 0), talkActions, rect);
        };
    }
    {
        auto& tradeAct = actionsRef.emplace_back();
        tradeAct.fText = eText::text(20, 1);
        tradeAct.fPress = [sellerId]() {
            eHoverWidget::sOpenMenu("", {});
            eGameWidget::sOpenSellerMenu(sellerId);
        };
    }
    {
        auto& cancelAct = actionsRef.emplace_back();
        cancelAct.fText = eText::text(20, 2);
        cancelAct.fPress = []() {
            eHoverWidget::sOpenMenu("", {});
        };
    }

    openMainMenu();
}

bool eMainCharAction::tryOpenTalk(
    const uint32_t sellerId,
    const std::string& baseName,
    const std::string& name,
    const SDL_Rect& rect) {
    const auto talk = mTalkHeard.nextUnheard(
        baseName, mQuests);
    if(!talk) return false;
    const auto& c = eTalks::get(*talk);
    const auto& text = eTalkText::text(c.fName);
    const auto closeAction =
        [this, sellerId, baseName, name, rect]() {
        const bool r = tryOpenTalk(
                sellerId, baseName, name, rect);
        if(r) return;
        openMainMenu(sellerId, baseName, name, rect);
    };
    eHoverWidget::sOpenTalk(text, closeAction, rect);
    mTalkHeard.setHeard(*talk, true);
    mServer->heardTalk(mClientId, *talk);
    return true;
}

void eMainCharAction::updateWalkRunSpeed() {
    const bool run = shouldRun();
    const float base = run ? 0.1f : 0.07f;
    const float speed = base*(1.f + mStats.fWalkRun);
    mMovementHandler.setSpeed(speed);
}

void eMainCharAction::mouseRelease(const ePointF& mousePos) {
    if(mClickAction) {
        mClickAction = false;
    } else if(mPressedUnit.expired() &&
              mPressedItem.expired() &&
              mPressedObject.expired()) {
        mMovementHandler.moveTo({mousePos});
    }
}

void eMainCharAction::stop() {
    mPressedUnit.reset();
    mPressedItem.reset();
    mPressedObject.reset();
    mPressedDoors.reset();
    mPressedStairs.reset();
    if(mAttackData.fType != eAttackTargetType::none) {
        stopAttack();
    }
    mMovementHandler.stopMoving();
}

bool eMainCharAction::rangedAttack(const eSkillChoice schoice) const {
    return mStats.rangedAttack(schoice);
}

void eMainCharAction::setSkillId(const eSkillChoice schoice, const int skillId) {
    auto& skillStats = mStats.skill(schoice);
    skillStats.fSkillId = skillId;
    recalculateStats();
}

void eMainCharAction::setAttributes(const eAttributes& attr) {
    mAttributes = attr;
    recalculateStats();
}

void eMainCharAction::setEquipment(const eEquipment& eq) {
    mEquipment = eq;
    recalculateStats();
}

void eMainCharAction::stand() {
    auto& model = mMainChar->model();

    const bool aggressive = model.aggressive();
    const int animId = eMovementHandler::sChooseAnim(
        mStandAnimId, mStandReadyAnimId, aggressive);

    mMainChar->setAnim(animId);
    mMainChar->incAnimId(1);
    mMainChar->setAnimSpeed(1.f);
    model.setAnimation(animId, 1.f);
}

bool eMainCharAction::shouldRun() const {
    if(!mRunning) return false;
    const float s = stamina();
    return s > 5.f || (s > 0.f && mContinueRunning);
}
