#include "eSlayerHelpers/ewaypoints.h"

eWaypoints eWaypoints::sWaypoints;

bool eWaypoints::known(
    const eAreaIds& area) const {
    for(const auto& w : *this) {
        if(w.fArea != area) continue;
        return w.fKnown;
    }
    return false;
}

bool eWaypoints::setKnown(
    const eAreaIds& area) {
    for(auto& w : *this) {
        if(w.fArea != area) continue;
        w.fKnown = true;
        return true;
    }
    return false;
}

void eWaypoints::initialize() {
    *this = sWaypoints;
}
