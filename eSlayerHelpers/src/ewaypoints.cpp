#include "eSlayerHelpers/ewaypoints.h"

eWaypoints eWaypoints::sWaypoints;

bool eWaypoints::known(
    const eAreaIds& area) {
    for(const auto& w : sWaypoints) {
        if(w.fArea != area) continue;
        return w.fKnown;
    }
    return false;
}

bool eWaypoints::setKnown(
    const eAreaIds& area) {
    for(auto& w : sWaypoints) {
        if(w.fArea != area) continue;
        w.fKnown = true;
        return true;
    }
    return false;
}

void eWaypoints::initialize() {
    *this = sWaypoints;
}
