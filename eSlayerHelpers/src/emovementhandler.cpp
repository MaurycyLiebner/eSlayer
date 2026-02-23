#include "eSlayerHelpers/emovementhandler.h"

#include "eSlayerHelpers/epathfinder.h"
#include "eSlayerHelpers/epathsmoother.h"
#include "eSlayerHelpers/evec2.h"

#include <cstdio>

void eMovementHandler::setWalkable(const eWalkable& w) {
    mWalkable = w;
}

bool eMovementHandler::moveTo(const ePointF& pos) {
    const int margin = mPathFindMargin;
    const double subdivision = mTileMoveSubdivision;
    const int dim = 2*margin + 1;
    ePathFinderMap map(0, 0, dim, dim);
    const auto iPos = (mPos * subdivision).round();
    for(int sx = 0; sx < dim; sx++) {
        for(int sy = 0; sy < dim; sy++) {
            const int x = (iPos.fX + sx - margin)/mTileMoveSubdivision;
            const int y = (iPos.fY + sy - margin)/mTileMoveSubdivision;
            map.set({sx, sy}, mWalkable(x, y));
        }
    }
    bool found;
    const ePoint from{margin, margin};
    const auto ipos = (pos * subdivision).round();
    const ePoint to{margin + (ipos.fX - iPos.fX),
                    margin + (ipos.fY - iPos.fY)};
    mPath = ePathFinder::findPath(map, from, to, found);
    for(auto& step : mPath) {
        step.fSrc.fX -= margin;
        step.fSrc.fY -= margin;
        step.fDst.fX -= margin;
        step.fDst.fY -= margin;
        step.fSrc.fX /= subdivision;
        step.fSrc.fY /= subdivision;
        step.fDst.fX /= subdivision;
        step.fDst.fY /= subdivision;
        step.fSrc.fX += std::round(mPos.fX*subdivision)/subdivision;
        step.fSrc.fY += std::round(mPos.fY*subdivision)/subdivision;
        step.fDst.fX += std::round(mPos.fX*subdivision)/subdivision;
        step.fDst.fY += std::round(mPos.fY*subdivision)/subdivision;
    }
    return found;
}

bool eMovementHandler::moveInDirection(const ePointF& pos) {
    eVec2d vec(pos.fX - mPos.fX,
               pos.fY - mPos.fY);
    vec.normalize(mSpeed);
    bool move = false;
    for(int i = 0; i < 90; i += 5) {
        for(int j : {-1, 1}) {
            if(i == 0 && j == 1) continue;
            auto v = vec;
            if(i != 0) v.rotate(i*j*5);
            const int x = std::floor(mPos.fX + v.x);
            const int y = std::floor(mPos.fY + v.y);
            move = mWalkable(x, y);
            if(move) {
                vec = v;
                break;
            }
        }
        if(move) {
            break;
        }
    }
    if(!move) return false;
    moveBy(vec);
    return true;
}

void eMovementHandler::moveBy(const eVec2d& vec) {
    mPos.fX += vec.x;
    mPos.fY += vec.y;
    mAngle = vec.angle();
}

bool eMovementHandler::increment() {
    if(mPath.empty()) return false;
    const auto iPos = mPos.floor();
    const int margin = 2;
    const int dim = 2*margin + 1;
    ePathFinderMap map(iPos.fX - margin, iPos.fY - margin, dim, dim);
    for(int x = iPos.fX - margin; x <= iPos.fX + margin; x++) {
        for(int y = iPos.fY - margin; y <= iPos.fY + margin; y++) {
            map.set({x, y}, mWalkable(x, y));
        }
    }
    int skipNodes;
    const auto vec = ePathSmoother::moveDir(
        mPath, map, mPos, 1., mSpeed, skipNodes);
    for(int i = 0; i < skipNodes && !mPath.empty(); i++) {
        mPath.erase(mPath.begin());
    }
    moveBy(vec);
    return true;
}
