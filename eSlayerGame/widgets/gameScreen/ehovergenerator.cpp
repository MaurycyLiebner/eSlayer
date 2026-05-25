#include "ehovergenerator.h"

#include "../../textures/etextgenerator.h"
#include "../epainter.h"
#include "../../elanguage.h"

#include <eSlayerHelpers/estringhelpers.h>
#include <eSlayerHelpers/eskills.h>

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
    case eModifierType::lightningResistance:
    case eModifierType::poisonResistance:
    case eModifierType::physicalResistance:

    case eModifierType::maxFireResistance:
    case eModifierType::maxColdResistance:
    case eModifierType::maxLightningResistance:
    case eModifierType::maxPoisonResistance:
    case eModifierType::maxPhysicalResistance:

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

    case eModifierType::coldLength:
    case eModifierType::freezeLength:

    case eModifierType::skillLevel:
        return true;

    case eModifierType::none:
    case eModifierType::count:

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

    case eModifierType::onAttack:
    case eModifierType::onStriking:
    case eModifierType::onKill:
    case eModifierType::onStruck:
    case eModifierType::onDeath:

    case eModifierType::fleshExplode:
    case eModifierType::iceExplode:
        return false;
    }
    return false;
}

void eHoverGenerator::addValue(SDL_Renderer* const r,
                               const int g, const int s,
                               const int min,
                               const int max,
                               const int skillId,
                               const eFontColor color,
                               const eModifierType type) {
    auto text = eLanguage::text(g, s);

    std::string minStr;
    if(addPlus(min, type)) minStr = "+";
    minStr += std::to_string(min);
    text = eStringHelpers::replaceAll(text, "%1", minStr);

    const std::string maxStr = std::to_string(max);
    text = eStringHelpers::replaceAll(text, "%2", maxStr);

    if(skillId >= 0) {
        const auto skillName = eSkills::sSkills.name(skillId);
        text = eStringHelpers::replaceAll(text, "%3", skillName);
    }

    addText(r, text, color);
}

void eHoverGenerator::addValue(SDL_Renderer* const r,
                               const int g, const int s,
                               const int min,
                               const int max,
                               const eFontColor color,
                               const eModifierType type) {
    addValue(r, g, s, min, max, -1, color, type);
}

void eHoverGenerator::addValue(SDL_Renderer* const r,
                               const int g, const int s,
                               const int value,
                               const eFontColor color,
                               const eModifierType type) {
    addValue(r, g, s, value, value, color, type);
}

std::shared_ptr<eTexture>
eHoverGenerator::generate(const eResolution& res,
                          SDL_Renderer* const r) const {
    const float mult = res.multiplier();
    const auto result = std::make_shared<eTexture>();
    const int fillMargin = 10*mult;
    result->create(r, maxWidth + 2*fillMargin,
                   totalHeight+2*fillMargin,
                   SDL_Color{0, 0, 0, 200});
    {
        const auto h = result->createTargetHolder(r);
        ePainter p(r);
        p.translate(fillMargin, fillMargin);
        int y = 0;
        for(const auto& l : lines) {
            p.drawTexture(maxWidth/2, y, l, eAlignment::hcenter);
            y += l->height();
        }
    }
    return result;
}

void eHoverGenerator::sPaint(const int w, const int h,
                             const int x, const int y,
                             const eResolution& res,
                             const std::shared_ptr<eTexture>& tex,
                             ePainter& p, const eAlignment align) {
    const float mult = res.multiplier();
    const int screenMargin = 40*mult;
    const int texW = tex->width();
    const int texH = tex->height();
    int drawX = x;
    int drawY = y;
    ePainter::drawCoordinates(drawX, drawY, texW, texH, align);
    if(drawX < screenMargin) {
        drawX = screenMargin;
    } else if(drawX + texW > w - screenMargin) {
        drawX = w - texW - screenMargin;
    }
    if(drawY < screenMargin) {
        drawY = screenMargin;
    } else if(drawY + texH > h - screenMargin) {
        drawY = h - texH - screenMargin;
    }
    p.drawTexture(drawX, drawY, tex);
}
