#include "eSlayerHelpers/echaracter.h"

#include "eSlayerHelpers/eweapontype.h"
#include "eSlayerHelpers/eitemsdata.h"

#include <algorithm>
#include <tinyxml2.h>
using namespace tinyxml2;

eCharacter::eCharacter(const std::string& name,
                       const bool hardcore) :
    mName(name), mHardcore(hardcore) {}

bool isTrue(std::string value) {
    std::transform(value.begin(), value.end(),
                   value.begin(), ::tolower);
    return value == "true";
}

bool eCharacter::load(const std::string& path,
                      eCharacter& c) {
    XMLDocument doc;
    const auto e = doc.LoadFile(path.c_str());
    if(e) {
        printf("Character XML LoadFile error %i reading %s.\n",
               e, path.c_str());
        return false;
    }
    const auto rootE = doc.RootElement();
    if(!rootE) {
        printf("Missing root element in %s.\n",
               path.c_str());
        return false;
    }
    const auto nameE = rootE->FirstChildElement("name");
    if(!nameE) {
        printf("Missing name element in %s.\n",
               path.c_str());
        return false;
    }
    const auto hardcoreE = rootE->FirstChildElement("hardcore");
    if(!hardcoreE) {
        printf("Missing hardcore element in %s.\n",
               path.c_str());
        return false;
    }
    const auto deadE = rootE->FirstChildElement("dead");
    if(!deadE) {
        printf("Missing dead element in %s.\n",
               path.c_str());
        return false;
    }
    c.mName = nameE->GetText();
    const std::string hardcoreV(hardcoreE->GetText());
    c.mHardcore = isTrue(hardcoreV);
    const std::string deadV(deadE->GetText());
    c.mDead = isTrue(deadV);

    int itemId = 0;
    const uint8_t amuletId = eItemsData::id("amulet3");
    eItem amulet{uint8_t(itemId++), amuletId, eItemType::amulet};
    {
        auto& mod = amulet.fModifiers.emplace_back();
        mod.fType = eModifierType::castRate;
        mod.fValue1 = 15.f;
    }
    {
        auto& mod = amulet.fModifiers.emplace_back();
        mod.fType = eModifierType::manaValue;
        mod.fValue1 = 75.f;
    }
    {
        auto& mod = amulet.fModifiers.emplace_back();
        mod.fType = eModifierType::lifeValue;
        mod.fValue1 = 20.f;
    }
    c.mEquipment.add(amulet);

    const uint8_t armorId = eItemsData::id("quilted_armor");
    eItem armor{uint8_t(itemId++), armorId, eItemType::armor};
    armor.fValue3 = 25.f;
    {
        auto& mod = armor.fModifiers.emplace_back();
        mod.fType = eModifierType::walkRun;
        mod.fValue1 = 1.5f;
    }
    {
        auto& mod = armor.fModifiers.emplace_back();
        mod.fType = eModifierType::defensePercent;
        mod.fValue1 = 0.75f;
    }
    {
        auto& mod = armor.fModifiers.emplace_back();
        mod.fType = eModifierType::lifeValue;
        mod.fValue1 = 20.f;
    }
    c.mEquipment.add(armor);

    const uint8_t swordId = eItemsData::id("short_sword");
    eItem sword{uint8_t(itemId++), swordId, eItemType::weapon,
                static_cast<uint8_t>(eWeaponSubtype::sword)};
    sword.fValue1 = 5.f;
    sword.fValue2 = 10.f;
    {
        auto& mod = sword.fModifiers.emplace_back();
        mod.fType = eModifierType::attackSpeed;
        mod.fValue1 = 1.5f;
    }
    {
        auto& mod = sword.fModifiers.emplace_back();
        mod.fType = eModifierType::damagePercent;
        mod.fValue1 = 0.75f;
        mod.fValue2 = 1.75f;
    }
    {
        auto& mod = sword.fModifiers.emplace_back();
        mod.fType = eModifierType::lifeValue;
        mod.fValue1 = 20.f;
    }
    c.mEquipment.add(sword);

    const uint8_t bowId = eItemsData::id("long_war_bow");
    eItem bow{uint8_t(itemId++), bowId, eItemType::weapon,
              static_cast<uint8_t>(eWeaponSubtype::bow)};
    bow.fValue1 = 5.f;
    bow.fValue2 = 10.f;
    {
        auto& mod = bow.fModifiers.emplace_back();
        mod.fType = eModifierType::attackSpeed;
        mod.fValue1 = 1.5f;
    }
    {
        auto& mod = bow.fModifiers.emplace_back();
        mod.fType = eModifierType::damagePoison;
        mod.fValue1 = 100.f;
        mod.fValue2 = 5.f;
    }
    {
        auto& mod = bow.fModifiers.emplace_back();
        mod.fType = eModifierType::damagePercent;
        mod.fValue1 = 0.75f;
        mod.fValue2 = 1.75f;
    }
    {
        auto& mod = bow.fModifiers.emplace_back();
        mod.fType = eModifierType::lifeValue;
        mod.fValue1 = 20.f;
    }
    c.mEquipment.add(bow);

    return true;
}

bool eCharacter::write(const std::string& path) const {
    XMLDocument doc;

    const auto decl = doc.NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
    doc.InsertFirstChild(decl);

    const auto rootE = doc.NewElement("character");
    doc.InsertEndChild(rootE);

    const auto nameE = rootE->InsertNewChildElement("name");
    nameE->SetText(mName.c_str());

    const auto hardcoreE = rootE->InsertNewChildElement("hardcore");
    hardcoreE->SetText(mHardcore ? "true" : "false");

    const auto deadE = rootE->InsertNewChildElement("dead");
    deadE->SetText(mDead ? "true" : "false");

    const auto e = doc.SaveFile(path.c_str());
    if(e) {
        eRuntimeThrow("Character XML SaveFile error " + std::to_string(e) +
                      " writing " + path);
        return false;
    }
    return true;
}
