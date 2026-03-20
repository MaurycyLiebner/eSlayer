#include "eskillbutton.h"

#include "../../textures/euitextures.h"

#include <eSlayerHelpers/eskills.h>

void eSkillButton::initialize() {
    setSkillId(mSkillId);
    fitContent();
}

void eSkillButton::setSkillId(const int skillId) {
    mSkillId = skillId;
    const auto& skill = eSkills::sSkills.get(skillId);
    const int iconId = skill.fIconId;
    const auto& icon = eUITextures::sSkillIcons.get(iconId);
    setTexture(icon);
}
