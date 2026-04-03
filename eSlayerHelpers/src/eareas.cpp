#include "eSlayerHelpers/eareas.h"

eAreas::eAreas(const int areaDim) :
    mAreaDim(areaDim) {}

eArea eAreas::posArea(const ePointF& pos) const {
    eArea result;
    if(mAreaDim > 0) {
        reinterpret_cast<ePoint&>(result) = pos.floor()/mAreaDim;
    } else {
        reinterpret_cast<ePoint&>(result) = (pos*(-mAreaDim)).floor();
    }
    return result;
}

ePointF eAreas::areaPos(const eArea& area) const {
    ePointF result;
    if(mAreaDim > 0) {
        result = {area.fX*float(mAreaDim),
                  area.fY*float(mAreaDim)};
    } else {
        result = {area.fX/float(mAreaDim),
                  area.fY/float(mAreaDim)};
    }
    return result;
}

void eAreas::clear() {
    mAreas.clear();
}

bool eAreas::hasArea(const eArea& tile) {
    const auto it = mAreas.find(tile);
    if(it == mAreas.end()) return false;
    return true;
}

void eAreas::erase(const eArea& area) {
    mAreas.erase(area);
}

void eAreas::emplace(const eArea& area) {
    mAreas.emplace(area);
}
