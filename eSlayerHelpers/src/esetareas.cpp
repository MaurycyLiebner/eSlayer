#include "eSlayerHelpers/esetareas.h"

eSetAreas::eSetAreas(const int areaDim) :
    mAreaDim(areaDim) {}

eArea eSetAreas::posArea(const ePointF& pos) const {
    eArea result;
    if(mAreaDim > 0) {
        reinterpret_cast<ePoint&>(result) = pos.floor()/mAreaDim;
    } else {
        reinterpret_cast<ePoint&>(result) = (pos*(-mAreaDim)).floor();
    }
    return result;
}

void eSetAreas::clear() {
    mAreas.clear();
}

bool eSetAreas::hasArea(const eArea& tile) {
    const auto it = mAreas.find(tile);
    if(it == mAreas.end()) return false;
    return true;
}

void eSetAreas::erase(const eArea& area, const int id) {
    mAreas[area].erase(id);
}

void eSetAreas::emplace(const eArea& area, const int id) {
    mAreas[area].emplace(id);
}

const std::set<int>& eSetAreas::at(const eArea& area) const {
    return mAreas[area];
}
