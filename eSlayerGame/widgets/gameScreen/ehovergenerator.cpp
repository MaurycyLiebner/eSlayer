#include "ehovergenerator.h"

#include "../../textures/etextgenerator.h"
#include "../epainter.h"
#include "../../elanguage.h"

#include <eSlayerHelpers/estringhelpers.h>

eHoverGenerator::eHoverGenerator(const eResolution& res) {
    const int fontSize = res.smallFontSize();
    font = eFonts::textFont(fontSize);
}

void eHoverGenerator::addSpace(SDL_Renderer* const r) {
    const auto space = std::make_shared<eTexture>();
    space->create(r, 1, font.fPtSize/2);
    totalHeight += space->height();
    lines.emplace_back(space);
}

void eHoverGenerator::addText(SDL_Renderer* const r,
                              const std::string& text,
                              const eFontColor color) {
    if(text.empty()) return;
    eTextGenerator gen(r, color, font);
    const auto tex = gen.generate(text);
    totalHeight += tex->height();
    maxWidth = std::max(maxWidth, tex->width());
    lines.emplace_back(tex);
}

bool addPlus(const float value,
             const eModifierType type) {
    if(value < 0.f) return false;
    switch(type) {
    case eModifierType::walkRun:

    case eModifierType::attackSpeed:
    case eModifierType::castRate:

    case eModifierType::defenseValue:
    case eModifierType::defensePercent:

    case eModifierType::damageValue:
    case eModifierType::damagePercent:

    case eModifierType::damagePoison:

    case eModifierType::attackRatingValue:
    case eModifierType::attackRatingPercent:

    case eModifierType::lifeValue:
    case eModifierType::lifePercent:

    case eModifierType::manaValue:
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

    case eModifierType::strength:
    case eModifierType::dexterity:
    case eModifierType::energy:
    case eModifierType::vitality:

    case eModifierType::meeleSplashDamage:

    case eModifierType::allSkills:

    case eModifierType::replenishLife:

    case eModifierType::fireSkillDamage:
    case eModifierType::coldSkillDamage:
    case eModifierType::lightningSkillDamage:
    case eModifierType::poisonSkillDamage:
        return true;

    case eModifierType::none:

    case eModifierType::damageFire:
    case eModifierType::damageLightning:
    case eModifierType::damageCold:

    case eModifierType::blockChancePercent:
    case eModifierType::blockRecoverySpeed:
    case eModifierType::hitRecoverySpeed:

    case eModifierType::lifeSteal:
    case eModifierType::manaSteal:

    case eModifierType::knockback:

    case eModifierType::regenerateMana:
        return false;
    }
    return false;
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

    case eModifierType::fireSkillDamage:
    case eModifierType::coldSkillDamage:
    case eModifierType::lightningSkillDamage:
    case eModifierType::poisonSkillDamage:
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

    case eModifierType::replenishLife:
    case eModifierType::regenerateMana:
        return eStringHelpers::floatToString(value);
    }
    return eStringHelpers::floatToString(value);
}

void eHoverGenerator::addValue(SDL_Renderer* const r,
                               const int g, const int s,
                               const float min,
                               const float max,
                               const eFontColor color,
                               const eModifierType type) {
    auto text = eLanguage::text(g, s);

    std::string minStr;
    if(addPlus(min, type)) minStr = "+";
    minStr += floatToString(min, type);
    text = eStringHelpers::replaceAll(text, "%1", minStr);

    const std::string maxStr = floatToString(max, type);
    text = eStringHelpers::replaceAll(text, "%2", maxStr);

    addText(r, text, color);
}

std::shared_ptr<eTexture>
eHoverGenerator::generate(SDL_Renderer* const r) const {
    const auto result = std::make_shared<eTexture>();
    result->create(r, maxWidth, totalHeight);
    {
        const auto h = result->createTargetHolder(r);
        ePainter p(r);
        int y = 0;
        for(const auto& l : lines) {
            p.drawTexture(maxWidth/2, y, l, eAlignment::hcenter);
            y += l->height();
        }
    }
    return result;
}

void eHoverGenerator::sPaint(const int w, const int h,
                             const int mouseX, const int mouseY,
                             const eResolution& res,
                             const std::shared_ptr<eTexture>& tex,
                             ePainter& p) {
    const int screenMargin = 40*res.multiplier();
    const int fillMargin = 10*res.multiplier();
    SDL_Rect rect{mouseX - tex->width()/2,
                  mouseY - tex->height(),
                  tex->width(), tex->height()};
    if(rect.y < screenMargin) rect.y = screenMargin;
    else if(rect.y + rect.h > h - screenMargin) rect.y = h - screenMargin - rect.h;
    if(rect.x < screenMargin) rect.x = screenMargin;
    else if(rect.x + rect.w > w - screenMargin) rect.x = w - screenMargin - rect.w;
    const SDL_Rect fillRect{rect.x - fillMargin, rect.y - fillMargin,
                            rect.w + 2*fillMargin, rect.h + 2*fillMargin};
    p.fillRect(fillRect, SDL_Color{0, 0, 0, 200});
    p.drawTexture(rect, tex, eAlignment::center);

}
