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
        button->setPressAction([this, skillId, &attrs, &eq]() {
            mStats->incSkillLevel(skillId);
            eGameWidget::sSendSkillLevelsChanged();
        });
        addWidget(button);
    }

    layoutVertically();
}
