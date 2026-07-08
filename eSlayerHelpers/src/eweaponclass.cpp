#include "eSlayerHelpers/eweaponclass.h"

#include "eSlayerHelpers/efileloaderbase.h"

eStringIdMapVector<eWeaponClass>
eWeaponClasses::sClasses;

void eWeaponClasses::load() {
    const auto dir = "Items";

    try {
        const auto jdata = eFileLoaderBase::parse(dir, "weaponClasses.json");
        for(auto it = jdata.begin(); it != jdata.end(); ++it) {
            const auto& key = it.key();
            const auto& value = it.value();

            eWeaponClass class_;
            const auto typeStr = value.value("type", "meele");
            class_.fType = eWeaponTypeHelpers::type(typeStr);
            class_.fTwoHanded = value.value("twoHanded", false);
            const auto secondHand = value.value(
                "secondHand", std::vector<std::string>());
            class_.fSecondHand.emplace_back(eItemType::none);
            for(const auto& typeStr : secondHand) {
                const auto type = eItemTypeHelpers::type(typeStr);
                class_.fSecondHand.emplace_back(type);
            }
            sClasses.add(key, class_);
        }
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/weaponClasses.json");
    }
}