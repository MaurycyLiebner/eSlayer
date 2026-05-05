#include "eSlayerHelpers/echaracter.h"

#include "eSlayerHelpers/epacket.h"
#include "eSlayerHelpers/eitemsdata.h"
#include "eSlayerHelpers/eskills.h"

#include <tinyxml2.h>
using namespace tinyxml2;

#include <algorithm>

eCharacter::eCharacter(const std::string& name,
                       const bool hardcore) :
    mName(name), mHardcore(hardcore) {}

bool isTrue(std::string value) {
    std::transform(value.begin(), value.end(),
                   value.begin(), ::tolower);
    return value == "true";
}

bool gReadModifier(eModifier& mod, const XMLElement* modE) {
    if(!modE) return false;

    const auto type = modE->Attribute("type");
    if(!type) return false;
    mod.typeFromKey(type);

    if(modE->Attribute("value")) {
        mod.fValue1 = modE->FloatAttribute("value");
        mod.fValue2 = mod.fValue1;
    } else {
        mod.fValue1 = modE->FloatAttribute("value1");
        mod.fValue2 = modE->FloatAttribute("value2");
    }

    return true;
}

bool gReadItem(eItem& item, const XMLElement* itemE) {
    if(!itemE) return false;

    const auto typeName = itemE->Attribute("type");
    if(!typeName) return false;
    const int dataId = eItemsData::id(typeName);
    item.fDataId = dataId;
    const auto& itemData = eItemsData::get(dataId);
    item.fType = itemData.fType;
    item.fSubType = itemData.fSubtype;

    item.fSockets = itemE->IntAttribute("sockets");

    item.fRequiredLevel = itemE->IntAttribute("requiredLevel");

    const auto rarityName = itemE->Attribute("rarity");
    if(!rarityName) return false;
    item.fRarity = eItemRarityHelpers::type(rarityName);

    item.fMinDmg = itemE->FloatAttribute("minDmg");
    item.fMaxDmg = itemE->FloatAttribute("maxDmg");
    item.fDefense = itemE->FloatAttribute("defense");
    item.fBlockChance = itemE->FloatAttribute("blockChance");

    auto modsE = itemE->FirstChildElement("modifiers");
    if(modsE) {
        auto modE = modsE->FirstChildElement("modifier");
        while(modE) {
            eModifier mod;
            if(gReadModifier(mod, modE)) {
                item.fModifiers.push_back(mod);
            }
            modE = modE->NextSiblingElement("modifier");
        }
    }

    return true;
}

bool gReadItemSlot(eItem& item,
                   const std::string& slotName,
                   const XMLElement* parentE) {
    if(!parentE) return false;

    const auto slotE = parentE->FirstChildElement(slotName.c_str());
    if(!slotE) {
        item = eItem{};
        return true;
    }

    const auto itemE = slotE->FirstChildElement("item");
    if(!itemE) {
        item = eItem{};
        return true;
    }

    return gReadItem(item, itemE);
}

bool gReadInventory(std::vector<eInventoryItem>& items,
                    const std::string& name,
                    const XMLElement* parentE) {
    if(!parentE) return false;

    const auto invE = parentE->FirstChildElement(name.c_str());
    if(!invE) return false;
    auto placeE = invE->FirstChildElement("place");
    while(placeE) {
        eInventoryItem inv{};

        inv.fX = placeE->IntAttribute("x");
        inv.fY = placeE->IntAttribute("y");
        inv.fW = placeE->IntAttribute("w");
        inv.fH = placeE->IntAttribute("h");

        const auto itemE = placeE->FirstChildElement("item");
        if(itemE) {
            gReadItem(inv.fItem, itemE);
        } else {
            inv.fItem = eItem{};
        }

        items.push_back(inv);
        placeE = placeE->NextSiblingElement("place");
    }

    return true;
}

bool gReadSkillLevels(const XMLElement* parentE,
                      eSkillLevels& skillLevels) {
    auto skillsE = parentE->FirstChildElement("skills");
    if(skillsE) {
        auto skillE = skillsE->FirstChildElement("skill");
        while (skillE) {
            const auto typeStr = skillE->Attribute("type");

            int level = 0;
            skillE->QueryIntAttribute("level", &level);

            if(typeStr && level > 0) {
                const int skillId = eSkills::sSkills.id(typeStr);
                skillLevels[skillId] = level - 1;
            }

            skillE = skillE->NextSiblingElement("skill");
        }
    }
    return true;
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

    // attributes
    if(const auto attrE = rootE->FirstChildElement("attributes")) {
        auto& attrs = c.mAttributes;
        const auto getAttr = [&](const std::string& name) {
            const auto ele = attrE->FirstChildElement(name.c_str());
            if(!ele) return 0;
            return ele->IntText();
        };
        attrs.fStrength  = getAttr("strength");
        attrs.fDexterity = getAttr("dexterity");
        attrs.fVitality  = getAttr("vitality");
        attrs.fEnergy    = getAttr("energy");
    }

    // skills
    gReadSkillLevels(rootE, c.mSkillLevels);

    // equipment
    if(const auto eqE = rootE->FirstChildElement("equipment")) {
        auto& eq = c.mEquipment;
        gReadItemSlot(eq.fBoots,   "boots",   eqE);
        gReadItemSlot(eq.fGloves,  "gloves",  eqE);
        gReadItemSlot(eq.fHelmet,  "helmet",  eqE);
        gReadItemSlot(eq.fArmor,   "armor",   eqE);
        gReadItemSlot(eq.fBelt,    "belt",    eqE);
        gReadItemSlot(eq.fRingL,   "ringL",   eqE);
        gReadItemSlot(eq.fRingR,   "ringR",   eqE);
        gReadItemSlot(eq.fAmulet,  "amulet",  eqE);
        gReadItemSlot(eq.fWeapon1L,"weapon1L",eqE);
        gReadItemSlot(eq.fWeapon1R,"weapon1R",eqE);
        gReadItemSlot(eq.fWeapon2L,"weapon2L",eqE);
        gReadItemSlot(eq.fWeapon2R,"weapon2R",eqE);
        gReadItemSlot(eq.fDragged, "dragged", eqE);

        gReadInventory(eq.fInventory, "inventory", eqE);
        gReadInventory(eq.fBeltPotions, "beltPotions", eqE);
        gReadInventory(eq.fBeltHiddenPotions, "beltPotionsHidden", eqE);
        gReadInventory(eq.fStash, "stash", eqE);
    }

    // int itemId = 0;
    // const uint8_t amuletId = eItemsData::id("amulet3");
    // eItem amulet{uint8_t(itemId++), amuletId, eItemType::amulet};
    // amulet.fRarity = eItemRarity::rare;
    // {
    //     auto& mod = amulet.fModifiers.emplace_back();
    //     mod.fType = eModifierType::castRate;
    //     mod.fValue1 = 15.f;
    // }
    // {
    //     auto& mod = amulet.fModifiers.emplace_back();
    //     mod.fType = eModifierType::manaValue;
    //     mod.fValue1 = 75.f;
    // }
    // {
    //     auto& mod = amulet.fModifiers.emplace_back();
    //     mod.fType = eModifierType::lifeValue;
    //     mod.fValue1 = 20.f;
    // }
    // c.mEquipment.add(amulet, true);

    // const uint8_t armorId = eItemsData::id("quilted_armor");
    // eItem armor{uint8_t(itemId++), armorId, eItemType::armor};
    // armor.fRarity = eItemRarity::unique;
    // armor.fSockets = 2;
    // armor.fDefense = 25.f;
    // {
    //     auto& mod = armor.fModifiers.emplace_back();
    //     mod.fType = eModifierType::walkRun;
    //     mod.fValue1 = 1.5f;
    // }
    // {
    //     auto& mod = armor.fModifiers.emplace_back();
    //     mod.fType = eModifierType::defensePercent;
    //     mod.fValue1 = 0.75f;
    // }
    // {
    //     auto& mod = armor.fModifiers.emplace_back();
    //     mod.fType = eModifierType::lifeValue;
    //     mod.fValue1 = 20.f;
    // }
    // c.mEquipment.add(armor, true);

    // const uint8_t swordId = eItemsData::id("short_sword");
    // eItem sword{uint8_t(itemId++), swordId, eItemType::weapon};
    // sword.fMinDmg = 5.f;
    // sword.fMaxDmg = 10.f;
    // {
    //     auto& mod = sword.fModifiers.emplace_back();
    //     mod.fType = eModifierType::attackSpeed;
    //     mod.fValue1 = 1.5f;
    // }
    // {
    //     auto& mod = sword.fModifiers.emplace_back();
    //     mod.fType = eModifierType::damagePercent;
    //     mod.fValue1 = 0.75f;
    //     mod.fValue2 = 1.75f;
    // }
    // {
    //     auto& mod = sword.fModifiers.emplace_back();
    //     mod.fType = eModifierType::lifeValue;
    //     mod.fValue1 = 20.f;
    // }
    // c.mEquipment.add(sword, true);

    // const uint8_t bowId = eItemsData::id("long_war_bow");
    // eItem bow{uint8_t(itemId++), bowId, eItemType::weapon};
    // bow.fMinDmg = 5.f;
    // bow.fMaxDmg = 10.f;
    // {
    //     auto& mod = bow.fModifiers.emplace_back();
    //     mod.fType = eModifierType::attackSpeed;
    //     mod.fValue1 = 1.5f;
    // }
    // {
    //     auto& mod = bow.fModifiers.emplace_back();
    //     mod.fType = eModifierType::damagePoison;
    //     mod.fValue1 = 100.f;
    //     mod.fValue2 = 5.f;
    // }
    // {
    //     auto& mod = bow.fModifiers.emplace_back();
    //     mod.fType = eModifierType::damagePercent;
    //     mod.fValue1 = 0.75f;
    //     mod.fValue2 = 1.75f;
    // }
    // {
    //     auto& mod = bow.fModifiers.emplace_back();
    //     mod.fType = eModifierType::lifeValue;
    //     mod.fValue1 = 20.f;
    // }
    // c.mEquipment.add(bow, true);

    return true;
}

void gWriteModifier(const eModifier& mod,
                    XMLElement* const e) {
    const auto modE = e->InsertNewChildElement("modifier");
    const auto type = mod.typeName();
    modE->SetAttribute("type", type.c_str());
    const int used = mod.valuesUsed();
    if(used == 1) {
        modE->SetAttribute("value", mod.fValue1);
    } else if(used == 2) {
        modE->SetAttribute("value1", mod.fValue1);
        modE->SetAttribute("value2", mod.fValue2);
    }
}

void gWriteItem(const eItem& item,
                XMLElement* const e) {
    const auto itemE = e->InsertNewChildElement("item");
    const int itemDataId = item.fDataId;
    const auto typeName = eItemsData::name(itemDataId);
    itemE->SetAttribute("type", typeName.c_str());
    const auto type = item.fType;
    switch(type) {
    case eItemType::potion:
        return;
    default:
        break;
    }

    itemE->SetAttribute("sockets", item.fSockets);
    itemE->SetAttribute("requiredLevel", item.fRequiredLevel);
    const auto rarityName = eItemRarityHelpers::name(item.fRarity);
    itemE->SetAttribute("rarity", rarityName.c_str());
    switch(type) {
    case eItemType::weapon:
    case eItemType::boots:
    case eItemType::shield:
        itemE->SetAttribute("minDmg", item.fMinDmg);
        itemE->SetAttribute("maxDmg", item.fMaxDmg);
        break;
    default:
        break;
    }
    switch(type) {
    case eItemType::armor:
    case eItemType::gloves:
    case eItemType::boots:
    case eItemType::helmet:
    case eItemType::shield:
    case eItemType::belt:
        itemE->SetAttribute("defense", item.fDefense);
        break;
    default:
        break;
    }
    switch(type) {
    case eItemType::shield:
        itemE->SetAttribute("blockChance", item.fBlockChance);
        break;
    default:
        break;
    }
    const auto modsE = itemE->InsertNewChildElement("modifiers");
    for(const auto& mod : item.fModifiers) {
        gWriteModifier(mod, modsE);
    }
}

void gWriteItemSlot(const eItem& item,
                    const std::string& slotName,
                    XMLElement* const e) {
    const auto slotE = e->InsertNewChildElement(slotName.c_str());
    if(item.fType == eItemType::none) return;
    gWriteItem(item, slotE);
}

void gWriteInventory(const std::vector<eInventoryItem>& items,
                     const std::string& name,
                     XMLElement* const e) {
    const auto invE = e->InsertNewChildElement(name.c_str());
    for(const auto& i : items) {
        const auto placeE = invE->InsertNewChildElement("place");
        placeE->SetAttribute("x", i.fX);
        placeE->SetAttribute("y", i.fY);
        placeE->SetAttribute("w", i.fW);
        placeE->SetAttribute("h", i.fH);
        gWriteItem(i.fItem, placeE);
    }
}

void gWriteSkillLevels(XMLElement* const e,
                       const eSkillLevels& skillLevels) {
    const auto skillsE = e->InsertNewChildElement("skills");
    for(const auto& s : skillLevels) {
        const auto skillE = skillsE->InsertNewChildElement("skill");
        const int skillId = s.first;
        if(skillId == 0) continue;
        const auto name = eSkills::sSkills.name(skillId);
        skillE->SetAttribute("type", name.c_str());
        const int level = s.second;
        skillE->SetAttribute("level", level + 1);
    }
}

bool eCharacter::write(const std::string& path,
                       const eEquipment& eq,
                       const eAttributes& attrs,
                       const eSkillLevels& skillLevels) const {
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

    const auto attrE = rootE->InsertNewChildElement("attributes");
    const auto strE = attrE->InsertNewChildElement("strength");
    strE->SetText(attrs.fStrength);
    const auto dexE = attrE->InsertNewChildElement("dexterity");
    dexE->SetText(attrs.fDexterity);
    const auto vitE = attrE->InsertNewChildElement("vitality");
    vitE->SetText(attrs.fVitality);
    const auto eneE = attrE->InsertNewChildElement("energy");
    eneE->SetText(attrs.fEnergy);

    gWriteSkillLevels(rootE, skillLevels);

    const auto eqE = rootE->InsertNewChildElement("equipment");
    gWriteItemSlot(eq.fBoots, "boots", eqE);
    gWriteItemSlot(eq.fGloves, "gloves", eqE);
    gWriteItemSlot(eq.fHelmet, "helmet", eqE);
    gWriteItemSlot(eq.fArmor, "armor", eqE);
    gWriteItemSlot(eq.fBelt, "belt", eqE);
    gWriteItemSlot(eq.fRingL, "ringL", eqE);
    gWriteItemSlot(eq.fRingR, "ringR", eqE);
    gWriteItemSlot(eq.fAmulet, "amulet", eqE);
    gWriteItemSlot(eq.fWeapon1L, "weapon1L", eqE);
    gWriteItemSlot(eq.fWeapon1R, "weapon1R", eqE);
    gWriteItemSlot(eq.fWeapon2L, "weapon2L", eqE);
    gWriteItemSlot(eq.fWeapon2R, "weapon2R", eqE);
    gWriteItemSlot(eq.fDragged, "dragged", eqE);
    gWriteInventory(eq.fInventory, "inventory", eqE);
    gWriteInventory(eq.fBeltPotions, "beltPotions", eqE);
    gWriteInventory(eq.fBeltHiddenPotions, "beltPotionsHidden", eqE);
    gWriteInventory(eq.fStash, "stash", eqE);

    const auto e = doc.SaveFile(path.c_str());
    if(e) {
        eRuntimeThrow("Character XML SaveFile error " + std::to_string(e) +
                      " writing " + path);
        return false;
    }
    return true;
}

void eCharacter::read(ePacket& p) {
    p >> mName;
    p >> mHardcore;
    p >> mDead;
    mEquipment.read(p);
    mAttributes.read(p);
    mSkillLevels.read(p);
}

void eCharacter::write(ePacket& p) const {
    p << mName;
    p << mHardcore;
    p << mDead;
    mEquipment.write(p);
    mAttributes.write(p);
    mSkillLevels.write(p);
}
