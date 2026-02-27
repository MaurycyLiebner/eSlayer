#include "eSlayerHelpers/emovementhandler.h"

#include "eSlayerHelpers/epathfinder.h"
#include "eSlayerHelpers/evec2.h"

#include <cstdio>

void eMovementHandler::intialize(const eWalkable& w,
                                 const eObsticle& o,
                                 const int charId) {
    mCharId = charId;
    mWalkable = w;
    mGoal.setWalkable(w);
    mObsticle = o;
}

bool eMovementHandler::moveTo(const ePointF& dst) {
    const ePointF pos = mPlanned.empty() ? mPos : mPlanned.back();
    const int margin = mPathFindMargin;
    const double subdivision = mTileMoveSubdivision;
    const int dim = 2*margin + 1;
    ePathFinderMap map(0, 0, dim, dim);
    const auto iPos = (pos * subdivision).round();
    for(int sx = 0; sx < dim; sx++) {
        for(int sy = 0; sy < dim; sy++) {
            const int x = (iPos.fX + sx - margin)/mTileMoveSubdivision;
            const int y = (iPos.fY + sy - margin)/mTileMoveSubdivision;
            map.set({sx, sy}, mWalkable(x, y));
        }
    }
    bool found;
    const ePoint from{margin, margin};
    const auto ipos = (dst * subdivision).round();
    const ePoint to{margin + (ipos.fX - iPos.fX),
                    margin + (ipos.fY - iPos.fY)};
    auto path = ePathFinder::findPath(map, from, to, found);
    for(auto& step : path) {
        step.fSrc.fX -= margin;
        step.fSrc.fY -= margin;
        step.fDst.fX -= margin;
        step.fDst.fY -= margin;
        step.fSrc.fX /= subdivision;
        step.fSrc.fY /= subdivision;
        step.fDst.fX /= subdivision;
        step.fDst.fY /= subdivision;
        step.fSrc.fX += std::round(pos.fX*subdivision)/subdivision;
        step.fSrc.fY += std::round(pos.fY*subdivision)/subdivision;
        step.fDst.fX += std::round(pos.fX*subdivision)/subdivision;
        step.fDst.fY += std::round(pos.fY*subdivision)/subdivision;
    }
    mGoal.moveOnPath(path);
    return found;
}

void eMovementHandler::moveInDirection(const ePointF& pos) {
    mGoal.moveInDir(pos);
}

void eMovementHandler::pushPlanned(std::queue<eIdPointF> planned) {
    while(!planned.empty()) {
        const auto& p = planned.front();
        if(p.fId >= mNextPlannedId) {
            mPlanned.push(p);
            mNextPlannedId = p.fId + 1;
        }
        planned.pop();
    }
}

void eMovementHandler::planMovement(const ePointF& to) {
    mPlanned.push({to, mNextPlannedId++});
}

bool eMovementHandler::incMovement(const double by) {
    bool moved = false;
    const double inc = mSpeed*by;
    double rem = inc;
    while(rem > 0 && mPlanned.size() > mDelay) {
        const auto& next = mPlanned.front();
        eVec2d vec{next.fX - mPos.fX, next.fY - mPos.fY};
        const double len = vec.length();
        if(len > rem && std::abs(len - rem) > 0.001) {
            vec.normalize(rem);
            rem = 0;
        } else {
            rem -= len;
            mPlanned.pop();
        }
        moved = tryMoveBy(vec) || moved;
    }
    return moved;
}

void eMovementHandler::clearPlanned() {
    std::queue<eIdPointF> tmp;
    std::swap(mPlanned, tmp);
}

bool eMovementHandler::tryMove(eVec2d& vec) {
    bool move = false;
    const double angleInc = 5.;
    const int iMax = std::ceil(mMaxDivergeAngle/angleInc);
    for(int i = 0; i <= iMax; i++) {
        for(int j : {-1, 1}) {
            if(i == 0 && j == 1) continue;
            auto v = vec;
            if(i != 0) v.rotate(i*angleInc*j*5);
            const ePointF newPos{mPos.fX + v.x,
                                 mPos.fY + v.y};
            const auto iNewPos = newPos.floor();
            move = mWalkable(iNewPos.fX, iNewPos.fY);
            if(move) {
                move = !mObsticle(mCharId, newPos);
                if(move) {
                    vec = v;
                    break;
                }
            }
        }
        if(move) {
            break;
        }
    }
    if(!move) return false;
    return true;
}

bool eMovementHandler::tryMoveBy(eVec2d vec) {
    const bool r = tryMove(vec);
    if(r) moveBy(vec);
    return r;
}

void eMovementHandler::moveBy(const eVec2d& vec) {
    mPos.fX += vec.x;
    mPos.fY += vec.y;
    mAngle = vec.angle();
}

bool eMovementHandler::increment(const double by) {
    bool result = false;
    if(mPlanned.size() <= mDelay && mGoal.moving()) {
        const double dist = mSpeed*by;
        const ePointF pos = mPlanned.empty() ? mPos : mPlanned.back();
        ePointF to;
        const bool move = mGoal.increment(pos, to, dist);
        if(move) {
            result = true;
            planMovement(to);
        }
    }
    return incMovement(by) || result;
}

void eMovementHandler::setDivergeAngle(const double a) {
    mMaxDivergeAngle = a;
}

void eMovementHandler::stopMoving() {
    clearPlanned();
    mGoal.stopMoving();
}
