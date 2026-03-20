#include "eskillbutton.h"

#include "../../textures/euitextures.h"

#include <eSlayerHelpers/eskills.h>

void eSkillButton::initialize() {
    const int attackSkillId = eSkills::sSkills.id("attack");
    setSkillId(attackSkillId);
    setNoPadding();
    fitContent();
}

void eSkillButton::setSkillId(const int skillId) {
    if(skillId == -1) return;
    mSkillId = skillId;
    const auto& skill = eSkills::sSkills.get(skillId);
    const int iconId = skill.fIconId;
    const auto& icon = eUITextures::sSkillIcons.get(iconId);
    setTexture(icon);
}
