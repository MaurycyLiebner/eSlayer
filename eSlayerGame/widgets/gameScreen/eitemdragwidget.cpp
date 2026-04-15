#include "eitemdragwidget.h"

#include "../../textures/eitemstextures.h"

#include "../../names/eitemnames.h"
#include "../../elanguage.h"
#include "ehovergenerator.h"
#include "../../names/eskillnames.h"

#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/estringhelpers.h>
#include <eSlayerHelpers/eattributes.h>
#include <eSlayerHelpers/estats.h>
#include <eSlayerHelpers/eskills.h>

eItemDragWidget* eItemDragWidget::sInstance = nullptr;

eItemDragWidget::eItemDragWidget(const eAttributes& attrs,
                                 const eStats& stats,
                                 eMainWindow* const w) :
    eWidget(w),
    mAttrs(attrs),
    mStats(stats) {
    sInstance = this;
}

eItemDragWidget::~eItemDragWidget() {
    sInstance = nullptr;
}

void eItemDragWidget::initialize(const eDropAction& dropAction) {
    mDropAction = dropAction;
}

void eItemDragWidget::setItemDataId(const int dataId) {
    if(dataId == -1) {
        mItem = nullptr;
    } else {
        const auto r = renderer();
        const auto& res = resolution();
        const auto name = eItemsData::name(dataId);
        auto& itemTex = eItemsTextures::get(name);
        itemTex.request(r, res);
        mItem = itemTex.fTex;
    }
}

void eItemDragWidget::setHoverItem(const eItem& item) {
    mHoverSkillId = -1;
    if(item.fType == eItemType::none) {
        mHover = nullptr;
    } else if(!mHover || item.fItemId != mHoverItemId) {
        const auto& res = resolution();
        const auto r = renderer();
        eHoverGenerator gen(res);

        {
            const auto name = eItemNames::name(item.fDataId);
            const auto socketsText = item.fSockets > 0 ?
                " [" + std::to_string(item.fSockets) + "]" :
                "";
            eFontColor color{eFontColor::normal};
            switch(item.fRarity) {
            case eItemRarity::normal:
                color = eFontColor::normal;
                break;
            case eItemRarity::magic:
                color = eFontColor::magic;
                break;
            case eItemRarity::rare:
                color = eFontColor::rare;
                break;
            case eItemRarity::set:
                color = eFontColor::set;
                break;
            case eItemRarity::unique:
                color = eFontColor::unique;
                break;
            }

            gen.addText(r, name + socketsText, color);
        }
        switch(item.fType) {
        case eItemType::armor:
        case eItemType::gloves:
        case eItemType::helmet:
        case eItemType::belt:
            gen.addValue(r, 6, 0, item.fValue3, item.fValue3, eFontColor::white);
            break;
        case eItemType::shield:
            gen.addValue(r, 6, 0, item.fValue3, item.fValue3, eFontColor::white);
            gen.addValue(r, 6, 2, item.fValue4, item.fValue4, eFontColor::white,
                     eModifierType::blockChancePercent);
            gen.addValue(r, 6, 1, item.fValue1, item.fValue2, eFontColor::white);
            break;
        case eItemType::boots:
            gen.addValue(r, 6, 0, item.fValue3, item.fValue3, eFontColor::white);
            gen.addValue(r, 6, 1, item.fValue1, item.fValue2, eFontColor::white);
            break;
        case eItemType::weapon:
            gen.addValue(r, 6, 1, item.fValue1, item.fValue2, eFontColor::white);
            break;
        default:
            break;
        }
        const auto& itemData = eItemsData::get(item.fDataId);
        const int level = std::max(itemData.fLevelReq, item.fRequiredLevel);
        if(level > 1) {
            const auto color = level > mAttrs.fLevel ?
                eFontColor::red : eFontColor::white;
            gen.addValue(r, 6, 4, level, level, color);
        }
        if(itemData.fStrengthReq > 0) {
            const int str = itemData.fStrengthReq;
            const auto color = str > mStats.fStrength ?
                eFontColor::red : eFontColor::white;
            gen.addValue(r, 6, 5, str, str, color);
        }
        if(itemData.fDexterityReq > 0) {
            const int dex = itemData.fDexterityReq;
            const auto color = dex > mStats.fDexterity ?
                eFontColor::red : eFontColor::white;
            gen.addValue(r, 6, 6, dex, dex, color);
        }
        if(itemData.fType == eItemType::weapon) {
            const float wsm = itemData.fWSM;
            int s;
            if(wsm < -20.f) {
                s = 11;
            } else if(wsm < -10.f) {
                s = 10;
            } else if(wsm < 0.f) {
                s = 9;
            } else if(wsm < 10.f) {
                s = 8;
            } else if(wsm < 20.f) {
                s = 7;
            } else {
                s = 7;
            }
            gen.addValue(r, 6, s, 0.f, 0.f, eFontColor::white);
        }
        for(const auto& mod : item.fModifiers) {
            const int s = static_cast<int>(mod.fType);
            gen.addValue(r, 10, s, mod.fValue1, mod.fValue2, eFontColor::blue, mod.fType);
        }
        if(item.fSockets > 0) {
            gen.addValue(r, 6, 3, item.fSockets, item.fSockets, eFontColor::blue);
        }

        mHover = gen.generate(r);
    }
    mHoverItemId = item.fItemId;
}

std::map<eModifierType, eModifier>
eItemDragWidget::calculateTotalModifiers(
    const int skillId, const int levelId,
    int& count, float& cooldown, float& manaCost) const {
    if(levelId < 0) return {};
    std::map<eModifierType, eModifier> result;
    const auto& skill = eSkills::sSkills.get(skillId);
    const auto& level = skill.skillLevel(levelId);
    count = level.fCount;
    cooldown = level.fCooldown;
    manaCost = level.fManaCost;
    result = level.fTotalModifiers;
    for(const auto& s : skill.fSynergies) {
        const int sSkillId = s.fSkillId;
        const int sLevelId = mStats.effectiveSkillLevel(sSkillId);
        if(sLevelId < 0) continue;
        const int maxLevel = s.fBoostLevels.size() - 1;
        const int sMaxLevelId = std::min(sLevelId, maxLevel);
        for(int level = 0; level <= sMaxLevelId; level++) {
            const auto& boost = s.fBoostLevels[level];
            count += boost.fCount;
            cooldown += boost.fCooldown;
            manaCost += boost.fManaCost;
            for(const auto& it : boost.fTotalModifiers) {
                const auto& mod = it.second;
                auto& dstMod = result[mod.fType];
                dstMod.fType = mod.fType;
                dstMod.fValue1 += mod.fValue1;
                dstMod.fValue2 += mod.fValue2;
            }
        }
    }
    return result;
}

void eItemDragWidget::setHoverSkill(
    const int skillId, const bool showNextLevel) {
    mHoverItemId = -1;
    if(skillId < 0) {
        mHover = nullptr;
    } else if(!mHover || skillId != mHoverSkillId) {
        const auto& skill = eSkills::sSkills.get(skillId);
        const auto name = eSkillNames::name(skillId);
        const auto desc = eSkillNames::description(skillId);
        const auto& res = resolution();
        const auto r = renderer();
        eHoverGenerator gen(res);
        gen.addText(r, name, eFontColor::green);
        gen.addText(r, desc, eFontColor::white);
        if(skillId != 0) {
            gen.addSpace(r);
            const int levelId = mStats.effectiveSkillLevel(skillId);
            if(levelId >= 0) {
                const auto& level = skill.skillLevel(levelId);
                gen.addValue(r, 13, 1, levelId + 1, levelId + 1,
                             eFontColor::white, eModifierType::none);
                int count;
                float cooldown;
                float manaCost;
                const auto mods = calculateTotalModifiers(
                    skillId, levelId, count, cooldown, manaCost);
                for(const auto& it : mods) {
                    const auto& mod = it.second;
                    const int s = static_cast<int>(mod.fType);
                    gen.addValue(r, 10, s, mod.fValue1, mod.fValue2,
                                 eFontColor::white, mod.fType);
                }
                if(manaCost != 0.f) {
                    gen.addValue(r, 13, 3, manaCost, manaCost,
                                 eFontColor::white, eModifierType::none);
                }
            }
            const int nextLevelId = levelId + 1;
            if(showNextLevel && nextLevelId >= 0) {
                if(levelId >= 0) gen.addSpace(r);
                const auto& level = skill.skillLevel(nextLevelId);
                gen.addValue(r, 13, 2, nextLevelId + 1, nextLevelId + 1,
                             eFontColor::white, eModifierType::none);
                int count;
                float cooldown;
                float manaCost;
                const auto mods = calculateTotalModifiers(
                    skillId, nextLevelId, count, cooldown, manaCost);
                for(const auto& it : mods) {
                    const auto& mod = it.second;
                    const int s = static_cast<int>(mod.fType);
                    gen.addValue(r, 10, s, mod.fValue1, mod.fValue2,
                                 eFontColor::white, mod.fType);
                }
                if(manaCost != 0.f) {
                    gen.addValue(r, 13, 3, manaCost, manaCost,
                                 eFontColor::white, eModifierType::none);
                }
            }

            if(showNextLevel && !skill.fSynergies.empty()) {
                bool addedSynergiesText = false;
                const auto textBase = eLanguage::text(13, 5);
                for(const auto& s : skill.fSynergies) {
                    const int sSkillId = s.fSkillId;
                    const int sLevelId = mStats.effectiveSkillLevel(sSkillId);
                    if(sLevelId + 1 >= s.fBoostLevels.size()) continue;
                    const auto sName = eSkillNames::name(sSkillId);
                    const auto sTextBase = eStringHelpers::replaceAll(textBase, "%1", sName);
                    if(!addedSynergiesText) {
                        addedSynergiesText = true;
                        auto text = eLanguage::text(13, 4);
                        text = eStringHelpers::replaceAll(text, "%1", name);
                        gen.addSpace(r);
                        gen.addText(r, text, eFontColor::green);
                    }
                    gen.addText(r, sTextBase, eFontColor::white);
                    const auto& sLevel = s.boostLevel(sLevelId + 1);
                    if(sLevel.fManaCost != 0.f) {
                        const auto manaCostFloatStr = eStringHelpers::floatToString(sLevel.fManaCost);
                        auto manaCostStr = eLanguage::text(13, 6);
                        manaCostStr = eStringHelpers::replaceAll(manaCostStr, "%1", manaCostFloatStr);
                        gen.addText(r, manaCostStr, eFontColor::white);
                    }
                    if(sLevel.fCooldown != 0.f) {
                        const auto cooldownFloatStr = eStringHelpers::floatToString(sLevel.fCooldown);
                        auto cooldownStr = eLanguage::text(13, 7);
                        cooldownStr = eStringHelpers::replaceAll(cooldownStr, "%1", cooldownFloatStr);
                        gen.addText(r, cooldownStr, eFontColor::white);
                    }
                    if(sLevel.fCount != 0) {
                        const auto countFloatStr = eStringHelpers::floatToString(sLevel.fCount);
                        auto countStr = eLanguage::text(13, 8);
                        countStr = eStringHelpers::replaceAll(countStr, "%1", countFloatStr);
                        gen.addText(r, countStr, eFontColor::white);
                    }
                    for(const auto& it : sLevel.fTotalModifiers) {
                        const auto& mod = it.second;
                        const int s = static_cast<int>(mod.fType);
                        gen.addValue(r, 10, s, mod.fValue1, mod.fValue2,
                                     eFontColor::white, mod.fType);
                    }
                }
            }
        }

        mHover = gen.generate(r);
    }
    mHoverSkillId = skillId;
}

void eItemDragWidget::sUpdateDragItem(const eEquipment& eq) {
    if(!sInstance) return;
    if(eq.fDragged.fType == eItemType::none) {
        sInstance->setItemDataId(-1);
    } else {
        const int dataId = eq.fDragged.fDataId;
        sInstance->setItemDataId(dataId);
    }
}

void eItemDragWidget::sSetHoverItem(const eItem& item) {
    if(!sInstance) return;
    sInstance->setHoverItem(item);
}

void eItemDragWidget::sSetHoverSkill(
    const int skillId, const bool showNextLevel) {
    if(!sInstance) return;
    sInstance->setHoverSkill(skillId, showNextLevel);
}

void eItemDragWidget::paintEvent(ePainter& p) {
    if(mItem) {
        p.drawTexture(mMousePos.x, mMousePos.y,
                      mItem, eAlignment::center);
    } else if(mHover) {
        const int h = height();
        const int w = width();
        const auto& res = resolution();
        const int mx = mMousePos.x;
        const int my = mMousePos.y;
        eHoverGenerator::sPaint(w, h, mx, my, res, mHover, p);
    }
}

bool eItemDragWidget::mouseMoveEvent(const eMouseEvent& e) {
    mMousePos.x = e.x();
    mMousePos.y = e.y();
    return mItem.get();
}

bool eItemDragWidget::mousePressEvent(const eMouseEvent& e) {
    if(!mItem) return false;
    mDropAction();
    return true;
}
