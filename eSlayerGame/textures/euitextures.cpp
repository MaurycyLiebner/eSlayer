#include "euitextures.h"

#include "../efileloader.h"
#include "etexture.h"
#include "espriteloader.h"

#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/eunitsinfo.h>

std::string eUITextures::sLoaded;
std::shared_ptr<eTexture>
eUITextures::sOkIcon;
std::shared_ptr<eTexture>
eUITextures::sCancelIcon;
eStringIdMapVector<std::shared_ptr<eTexture>>
eUITextures::sSkillIcons;
std::shared_ptr<eTexture>
eUITextures::sWalkIcon;
std::shared_ptr<eTexture>
eUITextures::sRunIcon;
std::shared_ptr<eTexture>
eUITextures::sPortalIcon;
std::shared_ptr<eTexture>
eUITextures::sInventoryIcon;
std::shared_ptr<eTexture>
eUITextures::sPartyIcon;
std::shared_ptr<eTexture>
eUITextures::sAttributesIcon;
std::shared_ptr<eTexture>
eUITextures::sSkillsIcon;
std::shared_ptr<eTexture>
eUITextures::sQuestsIcon;
std::shared_ptr<eTexture>
eUITextures::sMessagesIcon;
std::shared_ptr<eTexture>
eUITextures::sAutomapIcon;
std::shared_ptr<eTexture>
eUITextures::sGameMenuIcon;
std::shared_ptr<eTexture>
eUITextures::sNewFalseIcon;
std::shared_ptr<eTexture>
eUITextures::sNewTrueIcon;

std::shared_ptr<eTexture>
eUITextures::sLifeBar1;
std::shared_ptr<eTexture>
eUITextures::sLifeBar2;

std::shared_ptr<eTexture>
eUITextures::sStaminaBar1;
std::shared_ptr<eTexture>
eUITextures::sStaminaBar2;

std::shared_ptr<eTexture>
eUITextures::sExpBar1;
std::shared_ptr<eTexture>
eUITextures::sExpBar2;

std::shared_ptr<eTexture>
eUITextures::sAmuletSlot;
std::shared_ptr<eTexture>
eUITextures::sArmorSlot;
std::shared_ptr<eTexture>
eUITextures::sBeltSlot;
std::shared_ptr<eTexture>
eUITextures::sBootsSlot;
std::shared_ptr<eTexture>
eUITextures::sGlovesSlot;
std::shared_ptr<eTexture>
eUITextures::sHelmetSlot;
std::shared_ptr<eTexture>
eUITextures::sRingSlot;
std::shared_ptr<eTexture>
eUITextures::sWeaponSlot;
std::shared_ptr<eTexture>
eUITextures::sEmptySlot;
std::shared_ptr<eTexture>
eUITextures::sCoins;

eTextureCollection
eUITextures::sBg;
eTextureCollection
eUITextures::sStats;

std::shared_ptr<eTexture>
eUITextures::sStatsPlusButton;
std::shared_ptr<eTexture>
eUITextures::sStatsPlusButtonHovered;

std::shared_ptr<eTexture>
eUITextures::sWeaponSwitch1;
std::shared_ptr<eTexture>
eUITextures::sWeaponSwitch2;

eStringIdMapVector<eQuestTextures>
eUITextures::sQuestIcons;

std::shared_ptr<eTexture>
eUITextures::sTalk;

std::shared_ptr<eTexture>
eUITextures::sSocket;

std::map<int, std::shared_ptr<eTexture>>
eUITextures::sPortraits;

void eUITextures::sLoad(SDL_Renderer* const r,
                        const eResolution& res) {
    const auto suffix = res.textureSuffix();
    if(sLoaded == suffix) return;
    sLoaded = suffix;

    const auto dir = "Textures";

    {
        {
            const auto path = "ui/lifeBar/lifeBar";
            eSpriteLoader loader(dir, path, res, r);
            sLifeBar1 = loader.load(0);
            sLifeBar2 = loader.load(1);
        }
        {
            const auto path = "ui/staminaBar/staminaBar";
            eSpriteLoader loader(dir, path, res, r);
            sStaminaBar1 = loader.load(0);
            sStaminaBar2 = loader.load(1);
        }
        {
            const auto path = "ui/experienceBar/experienceBar";
            eSpriteLoader loader(dir, path, res, r);
            sExpBar1 = loader.load(0);
            sExpBar2 = loader.load(1);
        }
    }

    sOkIcon = eFileLoader::readTexture(r, dir, "ui/ok" + suffix + ".png");
    sCancelIcon = eFileLoader::readTexture(r, dir, "ui/cancel" + suffix + ".png");

    sWalkIcon = eFileLoader::readTexture(r, dir, "ui/walk" + suffix + ".png");
    sRunIcon = eFileLoader::readTexture(r, dir, "ui/run" + suffix + ".png");

    sPortalIcon = eFileLoader::readTexture(r, dir, "ui/portal" + suffix + ".png");

    sInventoryIcon = eFileLoader::readTexture(r, dir, "ui/inventory" + suffix + ".png");
    sPartyIcon = eFileLoader::readTexture(r, dir, "ui/party" + suffix + ".png");
    sAttributesIcon = eFileLoader::readTexture(r, dir, "ui/attributes" + suffix + ".png");
    sSkillsIcon = eFileLoader::readTexture(r, dir, "ui/skills" + suffix + ".png");
    sQuestsIcon = eFileLoader::readTexture(r, dir, "ui/quests" + suffix + ".png");
    sMessagesIcon = eFileLoader::readTexture(r, dir, "ui/messages" + suffix + ".png");
    sAutomapIcon = eFileLoader::readTexture(r, dir, "ui/automap" + suffix + ".png");
    sGameMenuIcon = eFileLoader::readTexture(r, dir, "ui/gameMenu" + suffix + ".png");
    sNewFalseIcon = eFileLoader::readTexture(r, dir, "ui/new_button_false" + suffix + ".png");
    sNewTrueIcon = eFileLoader::readTexture(r, dir, "ui/new_button_true" + suffix + ".png");

    sAmuletSlot = eFileLoader::readTexture(r, dir, "ui/inventory/itemSlotAmulet" + suffix + ".png");
    sArmorSlot = eFileLoader::readTexture(r, dir, "ui/inventory/itemSlotArmor" + suffix + ".png");
    sBeltSlot = eFileLoader::readTexture(r, dir, "ui/inventory/itemSlotBelt" + suffix + ".png");
    sBootsSlot = eFileLoader::readTexture(r, dir, "ui/inventory/itemSlotBoots" + suffix + ".png");
    sGlovesSlot = eFileLoader::readTexture(r, dir, "ui/inventory/itemSlotGloves" + suffix + ".png");
    sHelmetSlot = eFileLoader::readTexture(r, dir, "ui/inventory/itemSlotHelmet" + suffix + ".png");
    sRingSlot = eFileLoader::readTexture(r, dir, "ui/inventory/itemSlotRing" + suffix + ".png");
    sWeaponSlot = eFileLoader::readTexture(r, dir, "ui/inventory/itemSlotWeapon" + suffix + ".png");
    sEmptySlot = eFileLoader::readTexture(r, dir, "ui/inventory/itemSlot" + suffix + ".png");
    sCoins = eFileLoader::readTexture(r, dir, "ui/inventory/coins" + suffix + ".png");

    {
        const auto bgPath = "ui/widgetBg/widgetBg";
        eSpriteLoader loader(dir, bgPath, res, r);
        for(int i = 0; i < 9; i++) {
            loader.load(i, sBg);
        }
    }
    {
        const auto bgPath = "ui/stats/statsBox";
        eSpriteLoader loader(dir, bgPath, res, r);
        for(int i = 0; i < 4; i++) {
            loader.load(i, sStats);
        }
    }
    sStatsPlusButton = eFileLoader::readTexture(r, dir, "ui/stats/plusButton" + suffix + ".png");
    sStatsPlusButtonHovered = eFileLoader::readTexture(r, dir, "ui/stats/plusButtonHovered" + suffix + ".png");

    sWeaponSwitch1 = eFileLoader::readTexture(r, dir, "ui/inventory/weaponSwitch1" + suffix + ".png");
    sWeaponSwitch2 = eFileLoader::readTexture(r, dir, "ui/inventory/weaponSwitch2" + suffix + ".png");

    {
        const auto path = "ui/skills/skills.json";
        const auto jdata = eFileLoader::parse(dir, path);
        const auto names = jdata.get<std::vector<std::string>>();

        sSkillIcons.clear();
        for(const auto& name : names) {
            const auto path = "ui/skills/" + name + suffix + ".png";
            const auto tex = eFileLoader::readTexture(r, dir, path);
            sSkillIcons.add(name, tex);
        }

        for(const auto& it : eSkills::sSkills) {
            auto& skill = it.fValue;
            skill.fIconId = sSkillIcons.id(skill.fIcon);
        }
    }

    {
        const auto path = "ui/skills/skills.json";
        const auto jdata = eFileLoader::parse(dir, path);
        const auto names = jdata.get<std::vector<std::string>>();

        sSkillIcons.clear();
        for(const auto& name : names) {
            const auto path = "ui/skills/" + name + suffix + ".png";
            const auto tex = eFileLoader::readTexture(r, dir, path);
            sSkillIcons.add(name, tex);
        }

        for(const auto& it : eSkills::sSkills) {
            auto& skill = it.fValue;
            skill.fIconId = sSkillIcons.id(skill.fIcon);
        }
    }

    {
        const auto path = "ui/quests/quests.json";
        const auto jdata = eFileLoader::parse(dir, path);
        const auto names = jdata.get<std::vector<std::string>>();

        sQuestIcons.clear();
        for(const auto& name : names) {
            eQuestTextures texs;
            {
                const auto path = "ui/quests/" + name +
                                  "_not_started" + suffix + ".png";
                texs.fNotStarted = eFileLoader::readTexture(r, dir, path);
            }
            {
                const auto path = "ui/quests/" + name +
                                  "_started" + suffix + ".png";
                texs.fStarted = eFileLoader::readTexture(r, dir, path);
            }
            {
                const auto path = "ui/quests/" + name +
                                  "_finished" + suffix + ".png";
                texs.fFinished = eFileLoader::readTexture(r, dir, path);
            }
            sQuestIcons.add(name, texs);
        }
    }

    sTalk = eFileLoader::readTexture(r, dir, "ui/talk" + suffix + ".png");

    sSocket = eFileLoader::readTexture(r, dir, "ui/items/socket/socket" + suffix + ".png");

    for(const auto& it : eUnitsInfo::sUnits) {
        const auto name = it.fName;
        const auto path = "ui/portraits/" + name + suffix + ".png";
        const bool rr = eFileLoader::fileExists(dir, path);
        if(!rr) continue;
        const auto p = eFileLoader::readTexture(r, dir, path);
        sPortraits[it.fId] = p;
    }
}
