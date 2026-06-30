#include "eSlayerHelpers/eplacementhelper.h"

#include <vector>

#include <eSlayerHelpers/erand.h>

ePlacementHelper::ePlacementHelper() {}

void ePlacementHelper::randomize() {
    std::vector<std::pair<int, int>> vec(
        mMap.begin(), mMap.end());

    eRand::randomShuffle(vec);

    mMap.clear();
    mMap.insert(vec.begin(), vec.end());
}

void ePlacementHelper::add(
    const int id, const int count) {
    mMap.emplace(count, id);
}

int ePlacementHelper::get(int& area) {
    if(mMap.empty()) return -1;
    const auto end = mMap.end();
    const auto it = std::prev(end);
    const int id = it->second;
    area = it->first;
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
