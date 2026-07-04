#ifndef EUITEXTURES_H
#define EUITEXTURES_H

#include "../eresolution.h"
#include "etexturecollection.h"

#include <eSlayerHelpers/estringidmapvector.h>

#include <memory>

class SDL_Renderer;
class eTexture;

struct eQuestTextures {
    std::shared_ptr<eTexture> fNotStarted;
    std::shared_ptr<eTexture> fStarted;
    std::shared_ptr<eTexture> fFinished;
};

class eUITextures {
public:
    static void sLoad(SDL_Renderer* const r,
                      const eResolution& res);

    static std::shared_ptr<eTexture>
    sOkIcon;
    static std::shared_ptr<eTexture>
    sCancelIcon;

    static eStringIdMapVector<std::shared_ptr<eTexture>>
    sSkillIcons;
    static std::shared_ptr<eTexture>
    sWalkIcon;
    static std::shared_ptr<eTexture>
    sRunIcon;
    static std::shared_ptr<eTexture>
    sPortalIcon;
    static std::shared_ptr<eTexture>
    sInventoryIcon;
    static std::shared_ptr<eTexture>
    sPartyIcon;
    static std::shared_ptr<eTexture>
    sAttributesIcon;
    static std::shared_ptr<eTexture>
    sQuestsIcon;
    static std::shared_ptr<eTexture>
    sMessagesIcon;
    static std::shared_ptr<eTexture>
    sAutomapIcon;
    static std::shared_ptr<eTexture>
    sGameMenuIcon;
    static std::shared_ptr<eTexture>
    sNewFalseIcon;
    static std::shared_ptr<eTexture>
    sNewTrueIcon;
    static std::shared_ptr<eTexture>
    sSkillsIcon;
    static std::shared_ptr<eTexture>
    sLifeBar1;
    static std::shared_ptr<eTexture>
    sLifeBar2;
    static std::shared_ptr<eTexture>
    sStaminaBar1;
    static std::shared_ptr<eTexture>
    sStaminaBar2;
    static std::shared_ptr<eTexture>
    sExpBar1;
    static std::shared_ptr<eTexture>
    sExpBar2;

    static std::shared_ptr<eTexture>
    sAmuletSlot;
    static std::shared_ptr<eTexture>
    sArmorSlot;
    static std::shared_ptr<eTexture>
    sBeltSlot;
    static std::shared_ptr<eTexture>
    sBootsSlot;
    static std::shared_ptr<eTexture>
    sGlovesSlot;
    static std::shared_ptr<eTexture>
    sHelmetSlot;
    static std::shared_ptr<eTexture>
    sRingSlot;
    static std::shared_ptr<eTexture>
    sWeaponSlot;
    static std::shared_ptr<eTexture>
    sEmptySlot;
    static std::shared_ptr<eTexture>
    sCoins;

    static eTextureCollection sBg;
    static eTextureCollection sStats;

    static std::shared_ptr<eTexture>
    sStatsPlusButton;
    static std::shared_ptr<eTexture>
    sStatsPlusButtonHovered;

    static std::shared_ptr<eTexture>
    sWeaponSwitch1;
    static std::shared_ptr<eTexture>
    sWeaponSwitch2;

    static eStringIdMapVector<eQuestTextures>
    sQuestIcons;

    static std::shared_ptr<eTexture>
    sTalk;
private:
    static std::string sLoaded;
};


#endif // EUITEXTURES_H
