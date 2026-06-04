#include "eSlayerHelpers/eplacementhelper.h"

ePlacementHelper::ePlacementHelper() {}

void ePlacementHelper::add(
    const int id, const int count) {
    mMap.emplace(count, id);
}

int ePlacementHelper::get() {
    if(mMap.empty()) return -1;
    const auto end = mMap.end();
    const auto it = std::prev(end);
    const int id = it->second;
    return id;
}

void ePlacementHelper::set(const int id, const int count) {
    for(auto it = mMap.cbegin(); it != mMap.cend(); it++) {
        const int itId = it->second;
        if(itId != id) continue;
        mMap.erase(it);
        mMap.emplace(count, itId);
        break;
    }
}
