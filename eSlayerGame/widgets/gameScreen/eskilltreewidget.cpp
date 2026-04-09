#include "eskilltreewidget.h"

#include "eskillbutton.h"
#include "egamewidget.h"

#include <eSlayerHelpers/eskilltrees.h>
#include <eSlayerHelpers/estats.h>
#include <eSlayerHelpers/eattributes.h>
#include <eSlayerHelpers/eequipment.h>

void eSkillTreeWidget::initialize(
    const int skillTreeId, eStats& stats,
    const eAttributes& attrs, const eEquipment& eq) {
    mStats = &stats;

    const auto& skillTree = eSkillTrees::sTrees.get(skillTreeId);
    for(const int skillId : skillTree.fSkills) {
        const auto button = new eSkillButton(window());
        button->initialize();
        button->setSkillId(skillId);
        const int level = mStats->skillLevel(skillId);
        button->setBottomRightText(std::to_string(level + 1));
        button->setPressAction([this, button, skillId, &attrs, &eq]() {
            const int level = mStats->incSkillLevel(skillId);
            button->setBottomRightText(std::to_string(level + 1));
            eGameWidget::sSendSkillLevelsChanged();
        });
        addWidget(button);
    }

    layoutVertically();
}
