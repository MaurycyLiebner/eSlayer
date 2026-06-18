#include "eSlayerHelpers/echaracter.h"

#include "eSlayerHelpers/epacket.h"
#include "eSlayerHelpers/eitemsdata.h"
#include "eSlayerHelpers/eskills.h"
#include "eSlayerHelpers/eitemaffixes.h"
#include "eSlayerHelpers/estringhelpers.h"
#include "eSlayerHelpers/emapsettings.h"

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

    const auto used = mod.valuesUsed();
    if(used & eModValuesUsage::value1) {
        const auto name = mod.value1Name();
        mod.fValue1 = modE->IntAttribute(name.c_str());
    }
    if(used & eModValuesUsage::value2) {
        const auto name = mod.value2Name();
        mod.fValue2 = modE->IntAttribute(name.c_str());
    }
    if(used & eModValuesUsage::skillId) {
        const auto name = mod.skillName();
        const auto skillName = modE->Attribute(name.c_str());
        const int skillId = eSkills::sSkills.id(skillName);
        if(skillId < 0) eRuntimeThrow("Skill name \"" + skillName + "\" not recognized.");
        mod.fSkillId = skillId;
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

    item.fSockets = itemE->IntAttribute("sockets", 0);

    item.fRequiredLevel = itemE->IntAttribute("requiredLevel");

    const auto rarityName = itemE->Attribute("rarity");
    if(!rarityName) return false;
    item.fRarity = eItemRarityHelpers::type(rarityName);

    item.fMinDmg = itemE->FloatAttribute("minDmg");
    item.fMaxDmg = itemE->FloatAttribute("maxDmg");
    item.fDefense = itemE->FloatAttribute("defense");
    item.fBlockChance = itemE->FloatAttribute("blockChance");

    const auto prefixStr = itemE->Attribute("prefix");
    if(prefixStr) {
        item.fPrefix = eItemAffixes::sPrefixes.id(prefixStr);
    }

    const auto suffixStr = itemE->Attribute("suffix");
    if(suffixStr) {
        item.fSuffix = eItemAffixes::sSuffixes.id(suffixStr);
    }

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
                    uint32_t* const gold,
                    const std::string& name,
                    const XMLElement* parentE) {
    if(!parentE) return false;
    const auto invE = parentE->FirstChildElement(name.c_str());
    if(!invE) return false;
    if(gold) {
        *gold = 0;
        invE->QueryUnsignedAttribute("gold", gold);
    }
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
        const auto ptsE = skillsE->FirstChildElement("points");
        if(ptsE) {
            skillLevels.fRemainingPoints = ptsE->IntText(0);
        }
        auto skillE = skillsE->FirstChildElement("skill");
        while(skillE) {
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


void gReadSkillHotkeys(XMLElement* const e,
                       const std::string& leftRight,
                       std::map<int, int>& map) {
    const auto name = leftRight + "SkillHotkeys";
    const auto skillsE = e->FirstChildElement(name.c_str());
    if(skillsE) {
        auto skillE = skillsE->FirstChildElement("hotkey");
        while(skillE) {
            const auto typeStr = skillE->Attribute("skill");

            int key = 0;
            skillE->QueryIntAttribute("key", &key);

            if(typeStr && key > 0) {
                const int skillId = eSkills::sSkills.id(typeStr);
                map[key] = skillId;
            }

            skillE = skillE->NextSiblingElement("hotkey");
        }
    }
}

bool gReadSkill(XMLElement* const e,
                const std::string& leftRight,
                int& skillId) {
    const auto nameE = leftRight + "Skill";
    const auto skillE = e->FirstChildElement(nameE.c_str());
    if(!skillE) return false;
    const auto skillName = skillE->Attribute("skill");
    skillId = eSkills::sSkills.id(skillName);
    if(skillId < 0) skillId = 0;
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

    c.mName = nameE->GetText();
    const std::string hardcoreV(hardcoreE->GetText());
    c.mHardcore = isTrue(hardcoreV);

    // attributes
    if(const auto attrE = rootE->FirstChildElement("attributes")) {
        auto& attrs = c.mAttributes;
        const auto getAttr = [&](const std::string& name,
                                 const int default_) {
            const auto ele = attrE->FirstChildElement(name.c_str());
            if(!ele) return default_;
            return ele->IntText();
        };
        const auto getAttrF = [&](const std::string& name) {
            const auto ele = attrE->FirstChildElement(name.c_str());
            if(!ele) return 0.f;
            return ele->FloatText();
        };
        attrs.fLevel  = getAttr("level", 1);
        attrs.fExp  = getAttrF("experience");
        attrs.fStrength  = getAttr("strength", 1);
        attrs.fDexterity = getAttr("dexterity", 1);
        attrs.fVitality  = getAttr("vitality", 1);
        attrs.fEnergy    = getAttr("energy", 1);
        attrs.fStatPoints = getAttr("points", 0);
    }

    // skills
    gReadSkillLevels(rootE, c.mSkillLevels);
    gReadSkill(rootE, "left", c.mLeftSkill);
    gReadSkill(rootE, "right", c.mRightSkill);
    gReadSkill(rootE, "otherLeft", c.mOtherLeftSkill);
    gReadSkill(rootE, "otherRight", c.mOtherRightSkill);
    gReadSkillHotkeys(rootE, "left", c.mLeftHotkeys);
    gReadSkillHotkeys(rootE, "right", c.mRightHotkeys);

    const auto readEq = [](eBodyEquipment& eq,
                            XMLElement* const eqE) {
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
    };

    // equipment
    if(const auto eqE = rootE->FirstChildElement("equipment")) {
        auto& eq = c.mEquipment;
        const int active = eqE->IntAttribute("activeWeapons", 1);
        eq.fWeapons1 = active == 1;
        readEq(eq, eqE);
        gReadInventory(eq.fInventory, &eq.fInventoryGold,
                       "inventory", eqE);
        gReadInventory(eq.fBeltPotions, nullptr,
                       "beltPotions", eqE);
        gReadInventory(eq.fBeltHiddenPotions, nullptr,
                       "beltPotionsHidden", eqE);
        gReadInventory(eq.fStash, &eq.fStashGold,
                       "stash", eqE);
    }

    if(const auto eqE = rootE->FirstChildElement("bodyEquipment")) {
        auto& body = c.mBodies.emplace_back();
        readEq(body, eqE);
    }

    const auto getWaypointIds = [](const std::string& name,
                                   eWaypoint& w) {
        for(const auto& mit : eMapsSettings::sMaps) {
            const auto& m = mit.fValue;
            for(const auto& a : m.fAreas) {
                if(name == a.fName) {
                    w.fActId = m.fActId;
                    w.fMapId = mit.fId;
                    w.fAreaId = a.fId;
                    return true;
                }
            }
        }
        return false;
    };

    if(const auto wE = rootE->FirstChildElement("waypoints")) {
        auto aE = wE->FirstChildElement();
        while(aE) {
            auto wwE = aE->FirstChildElement();
            while(wwE) {
                const auto name = wwE->Name();
                eWaypoint w;
                wwE->QueryBoolAttribute("known", &w.fKnown);
                const bool r = getWaypointIds(name, w);
                if(r) {
                    c.mWaypoints.emplace_back(w);
                }
                wwE = wwE->NextSiblingElement();
            }
            aE = aE->NextSiblingElement();
        }
    }

    return true;
}

void gWriteModifier(const eModifier& mod,
                    XMLElement* const e) {
    const auto modE = e->InsertNewChildElement("modifier");
    const auto type = mod.typeName();
    modE->SetAttribute("type", type.c_str());
    const auto used = mod.valuesUsed();
    if(used & eModValuesUsage::value1) {
        const auto name = mod.value1Name();
        modE->SetAttribute(name.c_str(), mod.fValue1);
    }
    if(used & eModValuesUsage::value2) {
        const auto name = mod.value2Name();
        modE->SetAttribute(name.c_str(), mod.fValue2);
    }
    if(used & eModValuesUsage::skillId) {
        const auto name = mod.skillName();
        const auto skillName = eSkills::sSkills.name(mod.fSkillId);
        modE->SetAttribute(name.c_str(), skillName.c_str());
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

    if(item.fSockets != 0) {
        itemE->SetAttribute("sockets", item.fSockets);
    }
    if(item.fRequiredLevel != 0) {
        itemE->SetAttribute("requiredLevel", item.fRequiredLevel);
    }
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

    if(item.fPrefix) {
        const auto name = eItemAffixes::sPrefixes.name(item.fPrefix);
        itemE->SetAttribute("prefix", name.c_str());
    }

    if(item.fSuffix) {
        const auto name = eItemAffixes::sSuffixes.name(item.fSuffix);
        itemE->SetAttribute("suffix", name.c_str());
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
                     const std::optional<uint32_t>& gold,
                     const std::string& name,
                     XMLElement* const e) {
    const auto invE = e->InsertNewChildElement(name.c_str());
    if(gold) {
        invE->SetAttribute("gold", *gold);
    }
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
    const auto ptsE = skillsE->InsertNewChildElement("points");
    ptsE->SetText(skillLevels.fRemainingPoints);
    for(const auto& s : skillLevels) {
        const int skillId = s.first;
        if(skillId == 0) continue;
        const auto skillE = skillsE->InsertNewChildElement("skill");
        const auto name = eSkills::sSkills.name(skillId);
        skillE->SetAttribute("type", name.c_str());
        const int level = s.second;
        skillE->SetAttribute("level", level + 1);
    }
}

void gWriteSkillHotkeys(XMLElement* const e,
                        const std::string& leftRight,
                        const std::map<int, int>& map) {
    const auto name = leftRight + "SkillHotkeys";
    const auto skillsE = e->InsertNewChildElement(name.c_str());
    for(const auto& s : map) {
        const int hotkeyId = s.first;
        const int skillId = s.second;
        const auto skillE = skillsE->InsertNewChildElement("hotkey");
        const auto name = eSkills::sSkills.name(skillId);
        skillE->SetAttribute("skill", name.c_str());
        skillE->SetAttribute("key", hotkeyId);
    }
}

void gWriteSkill(XMLElement* const e,
                 const std::string& leftRight,
                 const int skillId) {
    const auto nameE = leftRight + "Skill";
    const auto skillE = e->InsertNewChildElement(nameE.c_str());
    const auto name = eSkills::sSkills.name(skillId);
    skillE->SetAttribute("skill", name.c_str());
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

    const auto attrE = rootE->InsertNewChildElement("attributes");
    const auto levelE = attrE->InsertNewChildElement("level");
    levelE->SetText(mAttributes.fLevel);
    const auto expE = attrE->InsertNewChildElement("experience");
    expE->SetText(mAttributes.fExp);
    const auto strE = attrE->InsertNewChildElement("strength");
    strE->SetText(mAttributes.fStrength);
    const auto dexE = attrE->InsertNewChildElement("dexterity");
    dexE->SetText(mAttributes.fDexterity);
    const auto vitE = attrE->InsertNewChildElement("vitality");
    vitE->SetText(mAttributes.fVitality);
    const auto eneE = attrE->InsertNewChildElement("energy");
    eneE->SetText(mAttributes.fEnergy);
    const auto ptsE = attrE->InsertNewChildElement("points");
    ptsE->SetText(mAttributes.fStatPoints);

    gWriteSkillLevels(rootE, mSkillLevels);
    gWriteSkill(rootE, "left", mLeftSkill);
    gWriteSkill(rootE, "right", mRightSkill);
    gWriteSkill(rootE, "otherLeft", mOtherLeftSkill);
    gWriteSkill(rootE, "otherRight", mOtherRightSkill);
    gWriteSkillHotkeys(rootE, "left", mLeftHotkeys);
    gWriteSkillHotkeys(rootE, "right", mRightHotkeys);

    const auto writeEq = [](const eBodyEquipment& eq,
                            XMLElement* const eqE) {
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
    };

    const auto eqE = rootE->InsertNewChildElement("equipment");
    eqE->SetAttribute("activeWeapons", mEquipment.fWeapons1 ? 1 : 2);
    writeEq(mEquipment, eqE);
    gWriteInventory(mEquipment.fInventory,
                    mEquipment.fInventoryGold,
                    "inventory", eqE);
    gWriteInventory(mEquipment.fBeltPotions,
                    std::nullopt,
                    "beltPotions", eqE);
    gWriteInventory(mEquipment.fBeltHiddenPotions,
                    std::nullopt,
                    "beltPotionsHidden", eqE);
    gWriteInventory(mEquipment.fStash,
                    mEquipment.fStashGold,
                    "stash", eqE);

    for(const auto& body : mBodies) {
        if(body.bodyEmpty()) continue;
        const auto eqE = rootE->InsertNewChildElement("bodyEquipment");
        writeEq(body, eqE);
        break;
    }

    const auto wE = rootE->InsertNewChildElement("waypoints");
    std::map<uint8_t, std::vector<eWaypoint>> acts;
    for(const auto& w : mWaypoints) {
        acts[w.fActId].emplace_back(w);
    }
    for(const auto& it : acts) {
        const auto actId = it.first;
        const auto ar = eStringHelpers::toRoman(actId);
        const auto aE = wE->InsertNewChildElement(ar.c_str());
        for(const auto& w : it.second) {
            const auto& minfo = eMapsSettings::sMaps.get(w.fMapId);
            const auto name = minfo.fAreas.name(w.fAreaId);
            const auto wE = aE->InsertNewChildElement(name.c_str());
            wE->SetAttribute("known", w.fKnown);
        }
    }

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
    mEquipment.read(p);
    mAttributes.read(p);
    mSkillLevels.read(p);

    uint8_t nBodies;
    p >> nBodies;
    for(int i = 0; i < nBodies; i++) {
        auto& b = mBodies.emplace_back();
        b.bodyRead(p);
    }
}

void eCharacter::write(ePacket& p) const {
    p << mName;
    p << mHardcore;
    mEquipment.write(p);
    mAttributes.write(p);
    mSkillLevels.write(p);

    const uint8_t nBodies = mBodies.size();
    p << nBodies;
    for(const auto& b : mBodies) {
        b.bodyWrite(p);
    }
}
