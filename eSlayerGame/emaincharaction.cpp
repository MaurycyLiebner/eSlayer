#include "emaincharaction.h"

#include "textures/echarstextures.h"
#include "textures/echartextures.h"
#include "units/eunit.h"

#include "widgets/gameScreen/einventorywidget.h"
#include "widgets/gameScreen/ehoverwidget.h"
#include "widgets/gameScreen/egamewidget.h"
#include "screens/egamescreen.h"

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
        const float size = info.fSize;
        const ePointF t{opos};
        const ePointF tr{opos.fX + 0.5f*size, opos.fY};
        const ePointF r{opos.fX + size, opos.fY};
        const ePointF br{opos.fX + size, opos.fY + 0.5f*size};
        const ePointF b{opos.fX + size, opos.fY + size};
        const ePointF bl{opos.fX + 0.5f*size, opos.fY + size};
        const ePointF l{opos.fX, opos.fY + size};
        const ePointF tl{opos.fX, opos.fY + 0.5f*size};
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
            if(info.fType == eObjectType::waypoint) {
                const auto areaId = mMap->areaAt(opos);
                eWaypoint::setKnown(mapId, areaId);
                const auto actId = mMap->actId();
                eGameScreen::sOpenWaypointMenu(actId, mapId, areaId);
            } else if(info.fType == eObjectType::stash) {
                eGameScreen::sOpenStash();
            } else if(info.fType == eObjectType::portal) {
                const auto portalId = object->fObjectId;
                const auto p = ePortal::portal(portalId);
                if(p) {
                    const bool camp = p->fCampPortalId == portalId;
                    eMoveToMapData moveData;
                    moveData.fType = eMoveToMapType::portal;
                    moveData.fMapId = camp ? p->fOutdoorMapId :
                        p->fCampMapId;
                    moveData.fAreaId = camp ? p->fOutdoorAreaId:
                        p->fCampAreaId;
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
            if(mPressedDoors) {
                const eServerDoors sdoors(mapId, *mPressedDoors);
                mServer->triggerDoors(mClientId, sdoors);
            } else if(mPressedStairs) {
                const auto toMapId = mPressedStairs->fTargetMapId;
                eMoveToMapData moveData;
                moveData.fType = eMoveToMapType::entrance;
                moveData.fFromMapId = mapId;
                moveData.fMapId = toMapId;
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
            incStamina(-by * 0.1f);
            animId = mRunAnimId;
        } else {
            incStamina(by * 0.1f);
            animId = eMovementHandler::sChooseAnim(
                mWalkAnimId, mWalkReadyAnimId, aggressive);
        }
    } else {
        incStamina(by * 0.1f);
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

void eMainCharAction::incStamina(const float by) {
    mStamina = std::clamp(mStamina + by, 0.f, mMaxStamina);
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
    return mStamina > 5.f || (mStamina > 0.f && mContinueRunning);
}
