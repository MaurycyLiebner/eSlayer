#include "eSlayerHelpers/ewaypoints.h"

std::vector<eWaypoint> eWaypoint::sWaypoints;

bool eWaypoint::known(
    const eAreaIds& area) {
    for(const auto& w : sWaypoints) {
        if(w.fArea != area) continue;
        return w.fKnown;
    }
    return false;
}

bool eWaypoint::setKnown(
    const eAreaIds& area) {
    for(auto& w : sWaypoints) {
        if(w.fArea != area) continue;
        w.fKnown = true;
        return true;
    }
    return false;
}
