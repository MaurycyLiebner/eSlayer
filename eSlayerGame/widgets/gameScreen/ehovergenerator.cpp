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

void eHoverGenerator::addValue(SDL_Renderer* const r,
                               const int g, const int s,
                               const float min,
                               const float max,
                               const eFontColor color,
                               const eModifierType type) {
    auto text = eLanguage::text(g, s);
    text = eStringHelpers::replaceAll(text, "%1", floatToString(min, type));
    text = eStringHelpers::replaceAll(text, "%2", floatToString(max, type));
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
