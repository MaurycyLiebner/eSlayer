#include "ehoverwidget.h"

#include "../../names/eitemnames.h"
#include "../../etext.h"
#include "ehovergenerator.h"
#include "../../names/eskillnames.h"

#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/estringhelpers.h>
#include <eSlayerHelpers/eattributes.h>
#include <eSlayerHelpers/estats.h>
#include <eSlayerHelpers/eskills.h>

eHoverWidget* eHoverWidget::sInstance = nullptr;

eHoverWidget::eHoverWidget(const eAttributes& attrs,
                           const eStats& stats,
                           eMainWindow* const w) :
    eWidget(w),
    mAttrs(attrs),
    mStats(stats) {
    sInstance = this;
}

eHoverWidget::~eHoverWidget() {
    sInstance = nullptr;
}

void eHoverWidget::initialize(
    const eDropAction& dropAction) {
    mDropAction = dropAction;
}

void eHoverWidget::setGameTooltip(
    const std::string& text,
    const SDL_Rect& rect) {
    if(text.empty()) {
        mGameTooltip.clear();
    } else {
        mGameTooltip = {text};
    }
    mGameHoverRect = rect;
}

void eHoverWidget::setGameTooltip(
    const std::vector<std::string>& text,
    const SDL_Rect& rect) {
    mGameTooltip = text;
    mGameHoverRect = rect;
}

void eHoverWidget::setItem(const eItem& item) {
    if(item.fType == eItemType::none) {
        mItem.reset();
    } else {
        const auto r = renderer();
        const auto& res = resolution();
        mItem = eItemInstanceTexture(r, res, item);
    }
}

void eHoverWidget::setHoverItem(
    const eItem& item, const SDL_Rect& hoverRect) {
    mHoverSkillId = -1;
    mHoverRect = hoverRect;
    if(item.fType == eItemType::none) {
        mHover = nullptr;
    } else if(!mHover || item.fItemId != mHoverItemId) {
        const auto& res = resolution();
        const auto r = renderer();
        eHoverGenerator gen(res);

        {
            const auto name = eItemNames::name(item);
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

            gen.addText(r, name, color);
        }
        switch(item.fType) {
        case eItemType::armor:
        case eItemType::gloves:
        case eItemType::helmet:
        case eItemType::belt:
            gen.addValue(r, 6, 0, item.fDefense, eFontColor::white);
            break;
        case eItemType::shield:
            gen.addValue(r, 6, 0, item.fDefense, eFontColor::white);
            gen.addValue(r, 6, 2, item.fBlockChance, eFontColor::white,
                     eModifierType::blockChancePercent);
            gen.addValue(r, 6, 1, item.fMinDmg, item.fMaxDmg, eFontColor::white);
            break;
        case eItemType::boots:
            gen.addValue(r, 6, 0, item.fDefense, eFontColor::white);
            gen.addValue(r, 6, 1, item.fMinDmg, item.fMaxDmg, eFontColor::white);
            break;
        case eItemType::weapon:
            gen.addValue(r, 6, 1, item.fMinDmg, item.fMaxDmg, eFontColor::white);
            break;
        default:
            break;
        }
        const auto& itemData = eItemsData::get(item.fDataId);
        const int level = std::max(itemData.fLevelReq, item.fRequiredLevel);
        if(level > 1) {
            const auto color = level > mAttrs.fLevel ?
                eFontColor::red : eFontColor::white;
            gen.addValue(r, 6, 4, level, color);
        }
        if(itemData.fStrengthReq > 0) {
            const int str = itemData.fStrengthReq;
            const auto color = str > mStats.fStrength ?
                eFontColor::red : eFontColor::white;
            gen.addValue(r, 6, 5, str, color);
        }
        if(itemData.fDexterityReq > 0) {
            const int dex = itemData.fDexterityReq;
            const auto color = dex > mStats.fDexterity ?
                eFontColor::red : eFontColor::white;
            gen.addValue(r, 6, 6, dex, color);
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
            gen.addValue(r, 6, s, 0.f, eFontColor::white);
        }
        for(const auto& mod : item.fModifiers) {
            const int s = static_cast<int>(mod.fType);
            gen.addValue(r, 10, s, mod.fValue1, mod.fValue2,
                         mod.fSkillId, eFontColor::blue, mod.fType);
        }
        if(item.fSockets > 0) {
            gen.addValue(r, 6, 3, item.fSockets, eFontColor::blue);
        }

        mHover = gen.generate(res, r);
    }
    mHoverItemId = item.fItemId;
}

eModsCollection eHoverWidget::calculateTotalModifiers(
    const int skillId, const int levelId) const {
    if(levelId < 0) return {};
    eModsCollection result;
    const auto& skill = eSkills::sSkills.get(skillId);
    const auto& level = skill.skillLevel(levelId);
    result = level.fTotalModifiers;
    for(const auto& s : skill.fSynergies) {
        const int sSkillId = s.fSkillId;
        const int sLevelId = mStats.effectiveSkillLevel(sSkillId);
        if(sLevelId < 0) continue;
        const auto& boost = s.fBoostLevels[sLevelId];
        result.addBoost(boost.fTotalModifiers);
    }
    result.collapse();
    return result;
}

void eHoverWidget::setHoverSkill(
    const int skillId, const bool showNextLevel,
    const SDL_Rect& rect) {
    mHoverItemId = -1;
    mHoverRect = rect;
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

        const auto printMods = [&](const eModsCollection& mods) {
            for(const auto& it : mods) {
                const auto& mod = it.second;
                const int s = static_cast<int>(mod.fType);
                gen.addValue(r, 10, s, mod.fValue1, mod.fValue2,
                             mod.fSkillId, eFontColor::white, mod.fType);
            }
        };

        float cooldown = 0.f;
        float radius = 0.f;
        int count = 0;
        float manaCost = 0.f;

        const auto printLevel = [&](const int levelId) {
            const auto mods = calculateTotalModifiers(
                skillId, levelId);
            printMods(mods);

            if(cooldown != mods.fCooldown) {
                cooldown = mods.fCooldown;
                const auto cooldownFloatStr = eStringHelpers::floatToString(cooldown);
                auto cooldownStr = eText::text(13, 11);
                cooldownStr = eStringHelpers::replaceAll(cooldownStr, "%1", cooldownFloatStr);
                gen.addText(r, cooldownStr, eFontColor::white);
            }

            if(radius != mods.fRadius) {
                radius = mods.fRadius;
                const auto radiusFloatStr = eStringHelpers::floatToString(radius);
                auto radiusStr = eText::text(13, 9);
                radiusStr = eStringHelpers::replaceAll(radiusStr, "%1", radiusFloatStr);
                gen.addText(r, radiusStr, eFontColor::white);
            }

            if(count != mods.fCount) {
                count = mods.fCount;
                const auto countFloatStr = eStringHelpers::floatToString(count);
                auto countStr = eText::text(13, 12);
                countStr = eStringHelpers::replaceAll(countStr, "%1", countFloatStr);
                gen.addText(r, countStr, eFontColor::white);
            }

            if(manaCost != mods.fManaCost) {
                manaCost = mods.fManaCost;
                const auto manaCostFloatStr = eStringHelpers::floatToString(manaCost);
                auto manaCostStr = eText::text(13, 3);
                manaCostStr = eStringHelpers::replaceAll(manaCostStr, "%1", manaCostFloatStr);
                gen.addText(r, manaCostStr, eFontColor::white);
            }
        };

        if(skillId != 0) {
            gen.addSpace(r);
            const int levelId = mStats.effectiveSkillLevel(skillId);
            if(levelId >= 0) {
                gen.addValue(r, 13, 1, levelId + 1,
                             eFontColor::white, eModifierType::none);
                printLevel(levelId);
            }
            const int nextLevelId = levelId + 1;
            if(showNextLevel && nextLevelId >= 0 &&
               nextLevelId < eSkills::sMaxSkillLevel) {
                if(levelId >= 0) gen.addSpace(r);
                gen.addValue(r, 13, 2, nextLevelId + 1,
                             eFontColor::white, eModifierType::none);
                printLevel(nextLevelId);
            }

            if(showNextLevel && !skill.fSynergies.empty()) {
                bool addedSynergiesText = false;
                const auto textBase = eText::text(13, 5);
                for(const auto& s : skill.fSynergies) {
                    const int sSkillId = s.fSkillId;
                    const int sLevelId = mStats.effectiveSkillLevel(sSkillId);
                    if(sLevelId + 1 >= s.fBoostLevels.size()) continue;
                    const auto sName = eSkillNames::name(sSkillId);
                    const auto sTextBase = eStringHelpers::replaceAll(textBase, "%1", sName);
                    if(!addedSynergiesText) {
                        addedSynergiesText = true;
                        auto text = eText::text(13, 4);
                        text = eStringHelpers::replaceAll(text, "%1", name);
                        gen.addSpace(r);
                        gen.addText(r, text, eFontColor::green);
                    }
                    gen.addText(r, sTextBase, eFontColor::white);
                    const auto& sLevel = s.boostLevel(sLevelId + 1);
                    const auto& mods = sLevel.fModifiers;

                    printMods(mods);

                    const float cooldown = mods.fCooldown;
                    if(cooldown != 0.f) {
                        const auto cooldownFloatStr = eStringHelpers::floatToStringWithSign(cooldown);
                        auto cooldownStr = eText::text(13, 7);
                        cooldownStr = eStringHelpers::replaceAll(cooldownStr, "%1", cooldownFloatStr);
                        gen.addText(r, cooldownStr, eFontColor::white);
                    }

                    const float radius = mods.fRadius;
                    if(radius != 0.f) {
                        const auto radiusFloatStr = eStringHelpers::floatToStringWithSign(radius);
                        auto radiusStr = eText::text(13, 10);
                        radiusStr = eStringHelpers::replaceAll(radiusStr, "%1", radiusFloatStr);
                        gen.addText(r, radiusStr, eFontColor::white);
                    }

                    const int count = mods.fCount;
                    if(count != 0) {
                        const auto countFloatStr = eStringHelpers::floatToStringWithSign(count);
                        auto countStr = eText::text(13, 8);
                        countStr = eStringHelpers::replaceAll(countStr, "%1", countFloatStr);
                        gen.addText(r, countStr, eFontColor::white);
                    }

                    const float manaCost = mods.fManaCost;
                    if(manaCost != 0.f) {
                        const auto manaCostFloatStr = eStringHelpers::floatToStringWithSign(manaCost);
                        auto manaCostStr = eText::text(13, 6);
                        manaCostStr = eStringHelpers::replaceAll(manaCostStr, "%1", manaCostFloatStr);
                        gen.addText(r, manaCostStr, eFontColor::white);
                    }
                }
            }
        }

        mHover = gen.generate(res, r);
    }
    mHoverSkillId = skillId;
}

void eHoverWidget::sUpdateDragItem(const eEquipment& eq) {
    if(!sInstance) return;
    sInstance->setItem(eq.fDragged);
}

void eHoverWidget::sSetHoverItem(
    const eItem& item, const SDL_Rect& rect) {
    if(!sInstance) return;
    sInstance->setHoverItem(item, rect);
}

void eHoverWidget::sSetHoverSkill(
    const int skillId, const bool showNextLevel,
    const SDL_Rect& rect) {
    if(!sInstance) return;
    sInstance->setHoverSkill(skillId, showNextLevel, rect);
}

void eHoverWidget::sSetGameTooltip(
    const std::string& text, const SDL_Rect& rect) {
    if(!sInstance) return;
    sInstance->setGameTooltip(text, rect);
}

void eHoverWidget::sSetGameTooltip(
    const std::vector<std::string>& text,
    const SDL_Rect& rect) {
    if(!sInstance) return;
    sInstance->setGameTooltip(text, rect);
}

void eHoverWidget::paintEvent(ePainter& p) {
    const auto& res = resolution();
    const float mult = res.multiplier();
    int mx = mMousePos.x;
    int my = mMousePos.y;
    const int w = width();
    const int h = height();
    eAlignment align = eAlignment::hcenter;
    const auto calcPos = [&](const SDL_Rect& hoverRect,
                             const bool alwaysTop) {
        const int margin = 10*mult;
        const int wx = hoverRect.x;
        const int wy = hoverRect.y;
        const int ww = hoverRect.w;
        const int wh = hoverRect.h;
        mx = wx + ww/2;
        const int h1 = wy;
        const int h2 = h - wy - wh;
        if(h1 > h2 || alwaysTop) {
            align = align | eAlignment::top;
            my = wy - margin;
        } else {
            align = align | eAlignment::bottom;
            my = wy + wh + margin;
        }
    };
    const auto item = mItem.request();
    if(item.fTex) {
        p.drawTexture(mx, my, item.fTex, eAlignment::center);
    } else if(mHover) {
        calcPos(mHoverRect, false);
        eHoverGenerator::sPaint(w, h, mx, my, res, mHover, p, align);
    } else {
        std::vector<std::string> tooltip;
        SDL_Rect hoverRect;

        if(const auto um = eWidget::sUnderMouse()) {
            const auto& t = um->tooltip();
            if(!t.empty()) {
                tooltip = {t};
                hoverRect = um->globalRect();
            }
        }

        bool alwaysTop = false;
        if(tooltip.empty()) {
            tooltip = mGameTooltip;
            hoverRect = mGameHoverRect;
            alwaysTop = true;
        }

        if(!tooltip.empty()) {
            if(tooltip != mTooltip) {
                const auto r = renderer();
                eHoverGenerator gen(res);
                for(const auto& t : tooltip) {
                    gen.addText(r, t, eFontColor::white);
                }
                mTooltipTex = gen.generate(res, r);
                mTooltip = tooltip;
            }

            calcPos(hoverRect, alwaysTop);
            const int h = height();
            const int w = width();
            eHoverGenerator::sPaint(w, h, mx, my, res, mTooltipTex, p, align);
        }
    }
}

bool eHoverWidget::mouseMoveEvent(const eMouseEvent& e) {
    mMousePos.x = e.x();
    mMousePos.y = e.y();
    return false;
}

bool eHoverWidget::mousePressEvent(const eMouseEvent& e) {
    if(!mItem) return false;
    mDropAction();
    return true;
}
