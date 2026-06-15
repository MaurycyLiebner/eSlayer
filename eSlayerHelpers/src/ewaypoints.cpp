#include "eSlayerHelpers/ewaypoints.h"

std::vector<eWaypoint> eWaypoint::sWaypoints;

bool eWaypoint::known(
    const uint8_t mapId,
    const uint8_t areaId) {
    for(const auto& w : sWaypoints) {
        if(w.fMapId != mapId) continue;
        if(w.fAreaId != areaId) continue;
        return w.fKnown;
    }
    return false;
}

bool eWaypoint::setKnown(
    const uint8_t mapId,
    const uint8_t areaId) {
    for(auto& w : sWaypoints) {
        if(w.fMapId != mapId) continue;
        if(w.fAreaId != areaId) continue;
        w.fKnown = true;
        return true;
    }
    return false;
}
