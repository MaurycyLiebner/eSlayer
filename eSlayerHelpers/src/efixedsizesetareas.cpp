#include "eSlayerHelpers/efixedsizesetareas.h"

void eFixedSizeSetAreas::initialize(
    const int width, const int height,
    const int areaDim) {
    mAreaDim = areaDim;
    if(mAreaDim > 0) {
        mWidth = (width + areaDim - 1)/areaDim;
        mHeight = (height + areaDim - 1)/areaDim;
    } else {
        mWidth = -mAreaDim*width;
        mHeight = -mAreaDim*height;
    }
    mAreas.resize(mHeight, std::vector<std::set<uint32_t>>(mWidth));
}

eArea eFixedSizeSetAreas::posArea(const ePointF& pos) const {
    eArea result;
    if(mAreaDim > 0) {
        reinterpret_cast<ePoint&>(result) = pos.floor()/mAreaDim;
    } else {
        reinterpret_cast<ePoint&>(result) = (pos*(-mAreaDim)).floor();
    }
    return result;
}

ePointF eFixedSizeSetAreas::areaPos(const eArea& area) const {
    ePointF result;
    if(mAreaDim > 0) {
        result = ePointF(area)*mAreaDim;
    } else {
        result = ePointF(area)/-mAreaDim;
    }
    return result;
}

void eFixedSizeSetAreas::clear() {
    for(int x = 0; x < mWidth; x++) {
        for(int y = 0; y < mHeight; y++) {
            mAreas[y][x].clear();
        }
    }
}

void eFixedSizeSetAreas::clear(const eArea& area) {
    mAreas[area.fY][area.fX].clear();
}

bool eFixedSizeSetAreas::hasArea(const eArea& area) const {
    if(area.fX < 0 || area.fY < 0) return false;
    if(area.fX >= mWidth || area.fY >= mHeight) return false;
    return true;
}

void eFixedSizeSetAreas::erase(const eArea& area, const uint32_t id) {
    mAreas[area.fY][area.fX].erase(id);
}

void eFixedSizeSetAreas::emplace(const eArea& area, const uint32_t id) {
    mAreas[area.fY][area.fX].emplace(id);
}

const std::set<uint32_t>& eFixedSizeSetAreas::at(const eArea& area) const {
    return mAreas[area.fY][area.fX];
}
