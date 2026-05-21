#include "eunit.h"

#include "../textures/echartextures.h"

void eUnit::setModel(const eCharUnitModel& model) {
    mModel = model;
    const auto& cmodel = model.model();
    const auto& data = cmodel.data();
    const auto& charData = data.charData();
    mBodyAnimId = charData.bodyAnimId();
}
