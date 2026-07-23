#include "eskillselectwidget.h"

#include "eskillbutton.h"

#include <eSlayerHelpers/eskills.h>

#include <cmath>

void eSkillSelectWidget::initialize(
    const std::vector<eSkillSelect>& skillIds,
    const eAlignment align,
    const eChooseAction& action,
    const eSkillChoice schoice) {
    setNoPadding();
    std::vector<eWidget*> rows;
    const int ncolumns = 4;
    const int nrows = (skillIds.size() + ncolumns - 1)/ncolumns;
    const bool canUseAura = schoice == eSkillChoice::right;
    int id = 0;
    for(int r = 0; r < nrows; r++) {
        const auto row = new eWidget(window());
        row->setNoPadding();
        for(int c = 0; c < ncolumns && id < skillIds.size();) {
            const auto& sskill = skillIds[id];
            const int skillId = sskill.fSkillId;
            const auto& skill = eSkills::sSkills.get(skillId);
            const auto type = skill.fType;
            const bool display = (type != eSkillType::aura || canUseAura) &&
                type != eSkillType::passive;
            if(!display) {
                id++;
                continue;
            }
            const auto button = new eSkillButton(window());
            button->initialize(static_cast<int>(schoice));
            button->setCanUse(sskill.fCanUse);
            button->setSkillId(skillId);
            button->setPressAction([this, action, skillId]() {
                action(skillId);
                deleteLater();
            });
            row->addWidget(button);
            id++;
            c++;
        }
        row->stackHorizontally();
        row->fitContent();
        prependWidget(row);
        rows.push_back(row);
    }

    stackVertically();
    fitContent();

    for(const auto row : rows) {
        row->align(align);
    }
}
