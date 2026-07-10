#include "eSlayerHelpers/eunitspecialdata.h"

#include "eSlayerHelpers/eunitdata.h"

void eUnitSpecialData::assign(const eUnitData& data) {
    fClientId = data.fCharId;
    fUnitType = data.fUnitInfoId;
    fPos = data.fPos;
    fMapId = data.fMapId;
    fAreaId = data.fAreaId;
    fHealth = data.fHealth;
    fMaxHealth = data.fMaxHealth;
}