#include "eskillselectwidget.h"

#include "eskillbutton.h"

#include <cmath>

void eSkillSelectWidget::initialize(const std::vector<int>& skillIds,
                                    const eAlignment align,
                                    const eChooseAction& action) {
    setNoPadding();
    std::vector<eWidget*> rows;
    const int ncolumns = 4;
    const int nrows = std::ceil(float(skillIds.size())/ncolumns);
    int id = 0;
    for(int r = 0; r < nrows; r++) {
        const auto row = new eWidget(window());
        row->setNoPadding();
        for(int c = 0; c < ncolumns && id < skillIds.size(); c++) {
            const auto skill = new eSkillButton(window());
            skill->initialize();
            const int skillId = skillIds[id];
            skill->setSkillId(skillId);
            skill->setPressAction([this, action, skillId]() {
                action(skillId);
                deleteLater();
            });
            row->addWidget(skill);
            id++;
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
