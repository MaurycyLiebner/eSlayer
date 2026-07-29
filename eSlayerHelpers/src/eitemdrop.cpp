#include "eSlayerHelpers/eitemdrop.h"

#include "eSlayerHelpers/eitemsdata.h"

void eItemDrop::read(const json& jitem) {
    const auto typeStr = jitem.value("type", "");
    const auto id = eItemsData::id(typeStr);
    if(id < 0) {
        eRuntimeThrow("Unrecognized item type \"" + typeStr + "\".");
    }
    fType = id;
    fChance = jitem.value("chance", 1.f);
}
