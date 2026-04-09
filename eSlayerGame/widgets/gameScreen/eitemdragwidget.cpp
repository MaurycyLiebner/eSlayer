#include "eitemdragwidget.h"

#include "../../textures/eitemstextures.h"
#include "../../textures/etextgenerator.h"

#include "../../names/eitemnames.h"
#include "../../elanguage.h"

#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/estringhelpers.h>
#include <eSlayerHelpers/eattributes.h>
#include <eSlayerHelpers/estats.h>

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
        const auto name = eItemsData::name(dataId);
        auto& itemTex = eItemsTextures::get(name);
        itemTex.request(r);
        mItem = itemTex.fTex;
    }
}

std::string floatToString(const float value,
                          const eModifierType type) {
    switch(type) {
    case eModifierType::walkRun:

    case eModifierType::attackSpeed:
    case eModifierType::castRate:

    case eModifierType::defensePercent:
    case eModifierType::damagePercent:
    case eModifierType::attackRatingPercent:
    case eModifierType::blockChancePercent:
    case eModifierType::blockRecoverySpeed:
    case eModifierType::hitRecoverySpeed:

    case eModifierType::lifePercent:
    case eModifierType::manaPercent:

    case eModifierType::pierceChance:

    case eModifierType::fireResistance:
    case eModifierType::coldResistance:
    case eModifierType::lightningResitance:
    case eModifierType::poisonResistance:

    case eModifierType::maxFireResistance:
    case eModifierType::maxColdResistance:
    case eModifierType::maxLightningResitance:
    case eModifierType::maxPoisonResistance:

    case eModifierType::lifeSteal:
    case eModifierType::manaSteal:

    case eModifierType::meeleSplashDamage:
    case eModifierType::knockback:
        return eStringHelpers::floatToString(100*value);

    case eModifierType::none:

    case eModifierType::defenseValue:
    case eModifierType::damageValue:

    case eModifierType::damageFire:
    case eModifierType::damageLightning:
    case eModifierType::damageCold:
    case eModifierType::damagePoison:

    case eModifierType::attackRatingValue:

    case eModifierType::lifeValue:
    case eModifierType::manaValue:

    case eModifierType::strength:
    case eModifierType::dexterity:
    case eModifierType::vitality:
    case eModifierType::energy:

    case eModifierType::allSkills:
        return eStringHelpers::floatToString(value);
    }
    return eStringHelpers::floatToString(value);
}

void eItemDragWidget::setHoverItem(const eItem& item) {
    if(item.fType == eItemType::none) {
        mHover = nullptr;
    } else if(!mHover || item.fItemId != mHoverItemId) {
        mHoverItemId = item.fItemId;
        int totalHeight = 0;
        int maxWidth = 0;
        std::vector<std::shared_ptr<eTexture>> lines;
        const auto r = renderer();
        const auto& res = resolution();
        const int fontSize = res.smallFontSize();
        const auto font = eFonts::textFont(fontSize);
        const auto addText = [&](const std::string& text,
                                 const eFontColor color) {
            if(text.empty()) return;
            eTextGenerator gen(r, color, font);
            const auto tex = gen.generate(text);
            totalHeight += tex->height();
            maxWidth = std::max(maxWidth, tex->width());
            lines.emplace_back(tex);
        };
        const auto addValue = [&](const int g, const int s,
                                  const float min,
                                  const float max,
                                  const eFontColor color,
                                  const eModifierType type = eModifierType::none) {
            auto text = eLanguage::text(g, s);
            text = eStringHelpers::replaceAll(text, "%1", floatToString(min, type));
            text = eStringHelpers::replaceAll(text, "%2", floatToString(max, type));
            addText(text, color);
        };

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

            addText(name + socketsText, color);
        }
        switch(item.fType) {
        case eItemType::armor:
        case eItemType::gloves:
        case eItemType::helmet:
        case eItemType::belt:
            addValue(6, 0, item.fValue3, item.fValue3, eFontColor::white);
            break;
        case eItemType::shield:
            addValue(6, 0, item.fValue3, item.fValue3, eFontColor::white);
            addValue(6, 2, item.fValue4, item.fValue4, eFontColor::white,
                     eModifierType::blockChancePercent);
            addValue(6, 1, item.fValue1, item.fValue2, eFontColor::white);
            break;
        case eItemType::boots:
            addValue(6, 0, item.fValue3, item.fValue3, eFontColor::white);
            addValue(6, 1, item.fValue1, item.fValue2, eFontColor::white);
            break;
        case eItemType::weapon:
            addValue(6, 1, item.fValue1, item.fValue2, eFontColor::white);
            break;
        default:
            break;
        }
        const auto& itemData = eItemsData::get(item.fDataId);
        const int level = std::max(itemData.fLevelReq, item.fRequiredLevel);
        if(level > 1) {
            const auto color = level > mAttrs.fLevel ?
                eFontColor::red : eFontColor::white;
            addValue(6, 4, level, level, color);
        }
        if(itemData.fStrengthReq > 0) {
            const int str = itemData.fStrengthReq;
            const auto color = str > mStats.fStrength ?
                eFontColor::red : eFontColor::white;
            addValue(6, 5, str, str, color);
        }
        if(itemData.fDexterityReq > 0) {
            const int dex = itemData.fDexterityReq;
            const auto color = dex > mStats.fDexterity ?
                eFontColor::red : eFontColor::white;
            addValue(6, 6, dex, dex, color);
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
            addValue(6, s, 0.f, 0.f, eFontColor::white);
        }
        for(const auto& mod : item.fModifiers) {
            const int s = static_cast<int>(mod.fType);
            addValue(10, s, mod.fValue1, mod.fValue2, eFontColor::blue, mod.fType);
        }
        if(item.fSockets > 0) {
            addValue(6, 3, item.fSockets, item.fSockets, eFontColor::blue);
        }

        mHover = std::make_shared<eTexture>();
        mHover->create(r, maxWidth, totalHeight);
        {
            const auto h = mHover->createTargetHolder(r);
            ePainter p(r);
            int y = 0;
            for(const auto& l : lines) {
                p.drawTexture(maxWidth/2, y, l, eAlignment::hcenter);
                y += l->height();
            }
        }
    }
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
    sInstance->setHoverItem(item);
}

void eItemDragWidget::paintEvent(ePainter& p) {
    if(mItem) {
        p.drawTexture(mMousePos.x, mMousePos.y,
                      mItem, eAlignment::center);
    } else if(mHover) {
        const int h = height();
        const int w = width();
        const auto& res = resolution();
        const int screenMargin = 40*res.multiplier();
        const int fillMargin = 10*res.multiplier();
        SDL_Rect rect{mMousePos.x - mHover->width()/2,
                      mMousePos.y - mHover->height(),
                      mHover->width(), mHover->height()};
        if(rect.y < screenMargin) rect.y = screenMargin;
        else if(rect.y + rect.h > h - screenMargin) rect.y = h - screenMargin - rect.h;
        if(rect.x < screenMargin) rect.x = screenMargin;
        else if(rect.x + rect.w > w - screenMargin) rect.x = w - screenMargin - rect.w;
        const SDL_Rect fillRect{rect.x - fillMargin, rect.y - fillMargin,
                                rect.w + 2*fillMargin, rect.h + 2*fillMargin};
        p.fillRect(fillRect, SDL_Color{0, 0, 0, 200});
        p.drawTexture(rect, mHover, eAlignment::center);
    }
}

bool eItemDragWidget::mouseMoveEvent(const eMouseEvent& e) {
    mMousePos.x = e.x();
    mMousePos.y = e.y();
    return mItem.get();
}

bool eItemDragWidget::mousePressEvent(const eMouseEvent& e) {
    if(!mItem) return false;
    mDropAction(mMousePos);
    return true;
}
