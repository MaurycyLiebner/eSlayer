#include "eskilltreewidget.h"

#include "eskillbutton.h"
#include "egamewidget.h"

#include <eSlayerHelpers/eskilltrees.h>
#include <eSlayerHelpers/estats.h>
#include <eSlayerHelpers/eattributes.h>
#include <eSlayerHelpers/eequipment.h>

void eSkillTreeWidget::initialize(
    const int skillTreeId, eStats& stats) {
    const auto& skillTree = eSkillTrees::sTrees.get(skillTreeId);
    for(const int skillId : skillTree.fSkills) {
        const auto button = new eSkillButton(window());
        button->initialize();
        button->setSkillId(skillId);
        const int level = stats.skillLevel(skillId);
        button->setBottomRightText(std::to_string(level + 1));
        button->setPressAction([&stats, button, skillId]() {
            const int level = stats.incSkillLevel(skillId);
            button->setBottomRightText(std::to_string(level + 1));
            eGameWidget::sSendSkillLevelsChanged();
        });
        addWidget(button);
    }

    layoutVertically();
}
