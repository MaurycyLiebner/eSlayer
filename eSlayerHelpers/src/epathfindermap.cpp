#include "../include/eSlayerHelpers/epathfindermap.h"

const int ePathFinderMap::sSubdivide = 2;

void ePathFinderMap::initialize(const int w, const int h,
                                const eWalkable& walkable) {
    const int sw = w*sSubdivide;
    const int sh = h*sSubdivide;
    mTiles = std::vector<std::vector<ePathFinderTile>>(
        sh, std::vector<ePathFinderTile>(sw));
    mWidth = sw;
    mHeight = sh;

    mWalkable = walkable;
}

bool ePathFinderMap::walkable(
    const ePoint& from,
    const int dx, const int dy) {
    if(from.fX < 0) return false;
    if(from.fY < 0) return false;
    if(from.fX >= mWidth) return false;
    if(from.fY >= mHeight) return false;
    auto& tile = mTiles[from.fY][from.fX];
    bool* ptr = nullptr;
    bool* iniPtr = nullptr;
    if(dx == 1 && dy == 0) {
        ptr = &tile.fBottomRight;
        iniPtr = &tile.fBottomRightInitialized;
    } else if(dx == 1 && dy == 1) {
        ptr = &tile.fBottom;
        iniPtr = &tile.fBottomInitialized;
    } else if(dx == 1 && dy == -1) {
        ptr = &tile.fRight;
        iniPtr = &tile.fRightInitialized;
    } else if(dx == 0 && dy == 1) {
        ptr = &tile.fBottomLeft;
        iniPtr = &tile.fBottomLeftInitialized;
    } else if(dx == 0 && dy == -1) {
        ptr = &tile.fTopRight;
        iniPtr = &tile.fTopRightInitialized;
    } else if(dx == -1 && dy == 0) {
        ptr = &tile.fTopLeft;
        iniPtr = &tile.fTopLeftInitialized;
    } else if(dx == -1 && dy == -1) {
        ptr = &tile.fTop;
        iniPtr = &tile.fTopInitialized;
    } else if(dx == -1 && dy == 1) {
        ptr = &tile.fLeft;
        iniPtr = &tile.fLeftInitialized;
    } else {
        return false;
    }
    if(!*iniPtr) {
        *iniPtr = true;
        const auto fromPos = tileToPos(from);
        const ePoint to{from.fX + dx, from.fY + dy};
        const auto toPos = tileToPos(to);
        *ptr = mWalkable(fromPos, toPos);
    }
    return *ptr;
}

void ePathFinderMap::erase(const ePoint& tile) {
    if(tile.fX < 0) return;
    if(tile.fY < 0) return;
    if(tile.fX >= mWidth) return;
    if(tile.fY >= mHeight) return;
    auto& t = mTiles[tile.fY][tile.fX];
    t.fTopInitialized = false;
    t.fTopRightInitialized = false;
    t.fRightInitialized = false;
    t.fBottomRightInitialized = false;
    t.fBottomInitialized = false;
    t.fBottomLeftInitialized = false;
    t.fLeftInitialized = false;
    t.fTopLeftInitialized = false;
}

void ePathFinderMap::nextIter() {
    mIter++;
}

int ePathFinderMap::distance(
    const ePoint& tile) const {
    if(tile.fX < 0) return std::numeric_limits<int>::max();
    if(tile.fY < 0) return std::numeric_limits<int>::max();
    if(tile.fX >= mWidth) return std::numeric_limits<int>::max();
    if(tile.fY >= mHeight) return std::numeric_limits<int>::max();
    const auto& t = mTiles[tile.fY][tile.fX];
    if(t.fIter != mIter) return std::numeric_limits<int>::max();
    return t.fDist;
}

void ePathFinderMap::setDistance(
    const ePoint& tile, const int dist) {
    if(tile.fX < 0) return;
    if(tile.fY < 0) return;
    if(tile.fX >= mWidth) return;
    if(tile.fY >= mHeight) return;
    auto& t = mTiles[tile.fY][tile.fX];
    t.fIter = mIter;
    t.fDist = dist;
}

bool ePathFinderMap::campAt(const ePointF& pos) const {
    return mCamp.campAt(pos);
}

bool ePathFinderMap::campAtLine(
    const ePointF& from, const ePointF& to) const {
    return mCamp.campAtLine(from, to);
}

void ePathFinderMap::addCampRect(const eRectF& rect) {
    mCamp.addRect(rect);
}

ePoint ePathFinderMap::posToTile(const ePointF& pos) {
    ePoint result;
    result.fX = std::round(pos.fX*sSubdivide);
    result.fY = std::round(pos.fY*sSubdivide);
    return result;
}

ePointF ePathFinderMap::tileToPos(const ePoint& tile) {
    ePointF result;
    result.fX = tile.fX/float(sSubdivide);
    result.fY = tile.fY/float(sSubdivide);
    return result;
}
