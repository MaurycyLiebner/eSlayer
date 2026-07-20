#include "eskilltreewidget.h"

#include "eskillbutton.h"
#include "egamewidget.h"

#include <eSlayerHelpers/eskilltrees.h>
#include <eSlayerHelpers/estats.h>
#include <eSlayerHelpers/eattributes.h>
#include <eSlayerHelpers/eequipment.h>

void eSkillTreeWidget::initialize(
    const int skillTreeId, eStats& stats) {
    setNoPadding();
    const auto& res = resolution();
    const float mult = res.multiplier();
    const int w = mult*250;
    const int h = mult*500;

    const auto& skillTree = eSkillTrees::sTrees.get(skillTreeId);

    int maxCol = 1;
    for(const auto& skill : skillTree.fSkills) {
        maxCol = std::max(maxCol, skill.fColumn);
    }

    std::vector<int> columnsMinY;
    columnsMinY.resize(maxCol + 1, 0);

    std::map<int, eSkillButton*> skillMap;

    for(const auto& skill : skillTree.fSkills) {
        const int skillId = skill.fSkillId;
        const auto button = new eSkillButton(window());
        skillMap[skillId] = button;
        button->initialize();
        button->setSkillId(skillId);
        const int level = stats.effectiveSkillLevel(skillId);
        button->setBottomRightText(std::to_string(level + 1));
        button->setPressAction([&stats, &skill, button, skillId]() {
            if(skill.fLevelReq > stats.fLevel) return;
            const int level = stats.incSkillLevel(skillId);
            button->setBottomRightText(std::to_string(level + 1));
            eGameWidget::sSendSkillLevelsChanged();
        });
        addWidget(button);
        const int bdim = button->width();
        const int colId = skill.fColumn;
        int& minY = columnsMinY[colId];
        button->setX(colId*(w - bdim)/maxCol);
        const float levelFrac = float(skill.fLevelReq)/skillTree.fMaxLevelReq;
        int y = levelFrac*(h - bdim);
        y = std::max(minY, y);
        button->setY(y);
        minY = y + 3.f*bdim/2.f;
    }

    for(const auto& skill : skillTree.fSkills) {
        const int skillId = skill.fSkillId;
        const auto b = skillMap[skillId];
        const auto& preq = skill.fPrerequisites;
        for(const auto p : preq) {
            const auto it = skillMap.find(p);
            if(it == skillMap.end()) continue;
            const auto req = it->second;
            mPrerequisites.emplace_back(b, req);
        }
    }

    resize(w, h);
}

void eSkillTreeWidget::paintEvent(ePainter& p) {
    const SDL_FColor color{1.f, 1.f, 1.f, 1.f};
    const auto& res = resolution();
    const float mult = res.multiplier();
    const float thick = 5*mult;
    for(const auto& pr : mPrerequisites) {
        const auto b1 = pr.first;
        const auto b2 = pr.second;
        const auto from = b2->center();
        const auto to = b1->center();
        const SDL_FPoint toX{to.x, from.y};
        const SDL_FPoint fromX{to.x, from.y - 0.5f*thick};
        p.drawLine(from, toX, thick, color);
        p.drawLine(fromX, to, thick, color);
    }
}
