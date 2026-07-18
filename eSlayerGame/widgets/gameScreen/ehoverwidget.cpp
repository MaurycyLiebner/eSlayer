#include "ehoverwidget.h"

#include "../../names/eitemnames.h"
#include "../../etext.h"
#include "ehovergenerator.h"
#include "../../names/eskillnames.h"
#include "../ecolors.h"
#include "../../textures/etextgenerator.h"
#include "../escrollwidget.h"
#include "../elabel.h"

#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/estringhelpers.h>
#include <eSlayerHelpers/eattributes.h>
#include <eSlayerHelpers/estats.h>
#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/erunsettings.h>
#include <eSlayerHelpers/eweaponclass.h>

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
    const eHoverItem& hitem,
    const SDL_Rect& hoverRect) {
    mHoverSkillId = -1;
    mHoverRect = hoverRect;
    const auto& item = hitem.fItem;
    if(item.fType == eItemType::none) {
        mHover = nullptr;
    } else if(!mHover || item.fItemId != mHoverItemId) {
        const auto& res = resolution();
        const auto r = renderer();
        eHoverGenerator gen(res);

        {
            uint32_t cost = 0;
            int s = -1;
            switch(hitem.fType) {
            case eHoverItemType::regular:
                break;
            case eHoverItemType::buy:
                s = 12;
                cost = item.calculateCost();
                break;
            case eHoverItemType::sell:
                s = 13;
                cost = item.calculateSellCost();
                break;
            }

            if(s >= 0) {
                auto text = eText::text(6, s);
                const auto costStr = std::to_string(cost);
                text = eStringHelpers::replaceAll(text, "%1", costStr);
                gen.addText(r, text, eFontColor::normal);
            }
        }

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
            const auto speedText = eText::text(6, s);
            const auto classStr = eItemNames::weaponClassName(itemData.fSubtype);
            auto classText = eText::text(6, 15);
            classText = eStringHelpers::replaceAll(classText, "%1", classStr);
            classText = eStringHelpers::replaceAll(classText, "%2", speedText);

            gen.addText(r, classText, eFontColor::white);
        }
        for(const auto& mod : item.fModifiers) {
            const int s = static_cast<int>(mod.fType);
            gen.addValue(r, 10, s, mod.fValue1, mod.fValue2,
                         mod.fSkillId, mod.fClassId,
                         eFontColor::blue, mod.fType);
        }
        if(itemData.fType == eItemType::potion) {
            if(itemData.fPotionFrameLength > 0.f) {
                auto text = eText::text(19, 0);
                const float durBase = itemData.fPotionFrameLength/25.f;
                const int dur10 = durBase*10;
                const float dur = dur10/10.f;
                const auto durStr = eStringHelpers::floatToString(dur);
                text = eStringHelpers::replaceAll(text, "%1", durStr);
                gen.addText(r, text, eFontColor::blue);
            }
            if(itemData.fPotionInstantHealth > 0.f) {
                auto text = eText::text(19, 1);
                const float instant = itemData.fPotionInstantHealth;
                const auto instantStr = eStringHelpers::floatToString(instant);
                text = eStringHelpers::replaceAll(text, "%1", instantStr);
                gen.addText(r, text, eFontColor::blue);
            }
            if(itemData.fPotionInstantMana > 0.f) {
                auto text = eText::text(19, 2);
                const float instant = itemData.fPotionInstantMana;
                const auto instantStr = eStringHelpers::floatToString(instant);
                text = eStringHelpers::replaceAll(text, "%1", instantStr);
                gen.addText(r, text, eFontColor::blue);
            }
            if(itemData.fPotionInstantStamina > 0.f) {
                auto text = eText::text(19, 3);
                const float instant = itemData.fPotionInstantStamina;
                const auto instantStr = eStringHelpers::floatToString(instant);
                text = eStringHelpers::replaceAll(text, "%1", instantStr);
                gen.addText(r, text, eFontColor::blue);
            }
            if(itemData.fPotionInstantHealthFrac > 0.f) {
                auto text = eText::text(19, 4);
                const int instant = 100*itemData.fPotionInstantHealthFrac;
                const auto instantStr = std::to_string(instant);
                text = eStringHelpers::replaceAll(text, "%1", instantStr);
                gen.addText(r, text, eFontColor::blue);
            }
            if(itemData.fPotionInstantManaFrac > 0.f) {
                auto text = eText::text(19, 5);
                const int instant = 100*itemData.fPotionInstantManaFrac;
                const auto instantStr = std::to_string(instant);
                text = eStringHelpers::replaceAll(text, "%1", instantStr);
                gen.addText(r, text, eFontColor::blue);
            }
            if(itemData.fPotionInstantStaminaFrac > 0.f) {
                auto text = eText::text(19, 6);
                const int instant = 100*itemData.fPotionInstantStaminaFrac;
                const auto instantStr = std::to_string(instant);
                text = eStringHelpers::replaceAll(text, "%1", instantStr);
                gen.addText(r, text, eFontColor::blue);
            }
            if(itemData.fPotionTotalHealth > 0.f) {
                auto text = eText::text(19, 7);
                const float total = itemData.fPotionTotalHealth;
                const auto totalStr = eStringHelpers::floatToString(total);
                text = eStringHelpers::replaceAll(text, "%1", totalStr);
                gen.addText(r, text, eFontColor::blue);
            }
            if(itemData.fPotionTotalMana > 0.f) {
                auto text = eText::text(19, 8);
                const float total = itemData.fPotionTotalMana;
                const auto totalStr = eStringHelpers::floatToString(total);
                text = eStringHelpers::replaceAll(text, "%1", totalStr);
                gen.addText(r, text, eFontColor::blue);
            }
            if(itemData.fPotionTotalStamina > 0.f) {
                auto text = eText::text(19, 9);
                const float total = itemData.fPotionTotalStamina;
                const auto totalStr = eStringHelpers::floatToString(total);
                text = eStringHelpers::replaceAll(text, "%1", totalStr);
                gen.addText(r, text, eFontColor::blue);
            }
            if(itemData.fPotionTotalHealthFrac > 0.f) {
                auto text = eText::text(19, 10);
                const int total = 100*itemData.fPotionTotalHealthFrac;
                const auto totalStr = std::to_string(total);
                text = eStringHelpers::replaceAll(text, "%1", totalStr);
                gen.addText(r, text, eFontColor::blue);
            }
            if(itemData.fPotionTotalManaFrac > 0.f) {
                auto text = eText::text(19, 11);
                const int total = 100*itemData.fPotionTotalManaFrac;
                const auto totalStr = std::to_string(total);
                text = eStringHelpers::replaceAll(text, "%1", totalStr);
                gen.addText(r, text, eFontColor::blue);
            }
            if(itemData.fPotionTotalStaminaFrac > 0.f) {
                auto text = eText::text(19, 12);
                const int total = 100*itemData.fPotionTotalStaminaFrac;
                const auto totalStr = std::to_string(total);
                text = eStringHelpers::replaceAll(text, "%1", totalStr);
                gen.addText(r, text, eFontColor::blue);
            }
            for(const auto& mod : itemData.fPotionMods) {
                const int s = static_cast<int>(mod.fType);
                gen.addValue(r, 10, s, mod.fValue1, mod.fValue2,
                             mod.fSkillId, mod.fClassId,
                             eFontColor::blue, mod.fType);
            }
        }
        if(item.fSockets > 0) {
            gen.addValue(r, 6, 3, item.fJewels.size(),
                         item.fSockets, eFontColor::blue);
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
                             mod.fSkillId, mod.fClassId,
                             eFontColor::white, mod.fType);
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

class eHoverLine : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const std::string& text,
                    const eAction& action,
                    const bool hover) {
        setNoPadding();

        mAction = action;
        const auto r = renderer();
        const auto& res = resolution();
        const int fontSize = res.smallFontSize();
        const auto font = eFonts::defaultFont(fontSize);
        {
            const auto color = hover ?
                eFontColor::white : eFontColor::gray;
            eTextGenerator gen(r, color, font);
            mNormal = gen.generate(text);
        }
        if(hover) {
            eTextGenerator gen(r, eFontColor::blue, font);
            mHover = gen.generate(text);
        }

        if(mNormal) {
            const int w = mNormal->width();
            const int h = mNormal->height();
            resize(w, h);
        }
    }
protected:
    void paintEvent(ePainter& p) override {
        if(!mHover || !hovered()) {
            if(mNormal) p.drawTexture(0, 0, mNormal);
        } else {
            p.drawTexture(0, 0, mHover);
        }
    }

    bool mouseReleaseEvent(const eMouseEvent& e) override {
        if(mAction) mAction();
        return true;
    }

    bool mousePressEvent(const eMouseEvent& e) override {
        return true;
    }
private:
    eAction mAction;
    std::shared_ptr<eTexture> mNormal;
    std::shared_ptr<eTexture> mHover;
};

class eHoverMenu : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const std::string& name,
                    const std::vector<eHoverAction>& actions) {
        setNoPadding();

        const auto inner = new eWidget(window());
        inner->setNoPadding();

        std::vector<eHoverLine*> lines;

        const auto l = new eHoverLine(window());
        l->initialize(name, nullptr, false);
        lines.emplace_back(l);
        inner->addWidget(l);

        for(const auto& a : actions) {
            const auto l = new eHoverLine(window());
            l->initialize(a.fText, a.fPress, true);
            inner->addWidget(l);
            lines.emplace_back(l);
        }

        inner->stackVertically();
        inner->fitContent();

        for(const auto l : lines) {
            l->align(eAlignment::hcenter);
        }

        addWidget(inner);

        const auto& res = resolution();
        const int p = res.largePadding();

        resize(inner->width() + 2*p,
               inner->height() + 2*p);
        inner->move(p, p);
    }
protected:
    void paintEvent(ePainter& p) override {
        const auto r = rect();
        p.fillRect(r, eColors::sHoverBg);
    }

    bool mousePressEvent(const eMouseEvent& e) override {
        return true;
    }

    bool mouseReleaseEvent(const eMouseEvent& e) override {
        return true;
    }
};

class eTalkWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const eAction& closeAction,
                    const std::string& text) {
        const auto& res = resolution();
        const int p = res.largePadding();
        const int w = res.centralWidgetSmallWidth();
        const int h = res.centralWidgetSmallHeight()/3;
        const auto label = new eLabel(window());
        label->setNoPadding();
        label->setWrapWidth(w);
        label->setText(text);
        label->setSmallFontSize();
        label->fitContent();
        mScroll = new eScrollWidget(window());
        mScroll->setNoPadding();
        mScroll->setScrollArea(label);
        mScroll->resize(w, h);
        addWidget(mScroll);
        resize(w + 2*p, h + 2*p);
        mScroll->move(p, p);
        mCloseAction = closeAction;
    }
protected:
    void paintEvent(ePainter& p) override {
        const auto r = rect();
        p.fillRect(r, eColors::sHoverBg);
        mDY += 8.f/eRunSettings::sFPS;
        mScroll->setDY(mDY);
        if(mDY > mScroll->dy() + 125) {
            if(mCloseAction) mCloseAction();
        }
    }

    bool mousePressEvent(const eMouseEvent& e) override {
        return true;
    }

    bool mouseReleaseEvent(const eMouseEvent& e) override {
        if(mCloseAction) mCloseAction();
        return true;
    }
private:
    float mDY = -20.f;
    eAction mCloseAction;
    eScrollWidget* mScroll = nullptr;
};

void position(eWidget* const w,
              const int padding,
              const SDL_Rect& rect) {
    const int x = std::max(padding, rect.x - (w->width() - rect.w)/2);
    const int y = std::max(padding, rect.y - w->height());
    w->move(x, y);
}

void eHoverWidget::openMenu(
    const std::string& name,
    const std::vector<eHoverAction>& actions,
    const SDL_Rect& rect) {
    if(mMenu) {
        mMenu->deleteLater();
        mMenu = nullptr;
    }
    if(mTalkWidget) {
        mTalkWidget->deleteLater();
        mTalkWidget = nullptr;
    }
    if(actions.empty()) {
        return;
    }

    mMenu = new eHoverMenu(window());
    mMenu->initialize(name, actions);
    addWidget(mMenu);
    const auto& res = resolution();
    const int p = res.largePadding();
    position(mMenu, p, rect);
}

void eHoverWidget::openTalk(
    const std::string& text,
    const eAction& closeAction,
    const SDL_Rect& rect) {
    if(mTalkWidget) {
        mTalkWidget->deleteLater();
        mTalkWidget = nullptr;
    }
    if(text.empty()) {
        if(mMenu) mMenu->show();
        return;
    }
    if(mMenu) mMenu->hide();

    mTalkWidget = new eTalkWidget(window());
    const auto closeActionV =
        closeAction ? closeAction : [this]() {
        eHoverWidget::sOpenTalk("");
    };
    mTalkWidget->initialize(closeActionV, text);
    addWidget(mTalkWidget);
    const auto& res = resolution();
    const int p = res.largePadding();
    position(mTalkWidget, p, rect);
}

void eHoverWidget::sUpdateDragItem(const eItem& item) {
    if(!sInstance) return;
    sInstance->setItem(item);
}

void eHoverWidget::sSetHoverItem(
    const eHoverItem& hitem, const SDL_Rect& rect) {
    if(!sInstance) return;
    sInstance->setHoverItem(hitem, rect);
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

void eHoverWidget::sOpenMenu(
    const std::string& name,
    const std::vector<eHoverAction>& actions,
    const SDL_Rect& rect) {
    if(!sInstance) return;
    sInstance->openMenu(name, actions, rect);
}

void eHoverWidget::sOpenTalk(
    const std::string& text,
    const eAction& closeAction,
    const SDL_Rect& rect) {
    if(!sInstance) return;
    sInstance->openTalk(text, closeAction, rect);
}

void eHoverWidget::sClearHover() {
    if(!sInstance) return;
    sInstance->mHover = nullptr;
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
