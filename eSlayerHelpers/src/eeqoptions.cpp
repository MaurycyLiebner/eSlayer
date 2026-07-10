#include "eSlayerHelpers/eeqoptions.h"

#include "eSlayerHelpers/evectorhelpers.h"
#include "eSlayerHelpers/eitem.h"

bool eEqOptions::validateItem(const eItem& item) const {
    const bool r = eVectorHelpers::contains(fTypes, item.fType);
    if(!r) return false;
    if(item.fType == eItemType::weapon) {
        const auto class_ = item.fSubType;
        const bool r = eVectorHelpers::contains(fWeaponClasses, class_);
        if(!r) return false;
    }
    return true;
}
