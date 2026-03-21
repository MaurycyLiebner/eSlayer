#include "eSlayerHelpers/eunitareas.h"

eUnitAreas::eUnitAreas(const int areaDim) :
    mAreaDim(areaDim) {}

eUnitArea eUnitAreas::posArea(const ePointF& pos) const {
    eUnitArea result;
    reinterpret_cast<ePoint&>(result) = pos.floor()/mAreaDim;
    return result;
}

void eUnitAreas::clear() {
    mAreas.clear();
}

bool eUnitAreas::hasArea(const eUnitArea& tile) {
    const auto it = mAreas.find(tile);
    if(it == mAreas.end()) return false;
    return true;
}

void eUnitAreas::erase(const eUnitArea& area, const int id) {
    mAreas[area].erase(id);
}

void eUnitAreas::emplace(const eUnitArea& area, const int id) {
    mAreas[area].emplace(id);
}

const std::set<int>& eUnitAreas::at(const eUnitArea& area) {
    return mAreas[area];
}
