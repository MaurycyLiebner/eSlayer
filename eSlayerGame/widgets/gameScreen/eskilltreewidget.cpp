#include "eskilltreewidget.h"

#include "eskillbutton.h"
#include "egamewidget.h"

#include <eSlayerHelpers/eskilltrees.h>
#include <eSlayerHelpers/estats.h>
#include <eSlayerHelpers/eattributes.h>
#include <eSlayerHelpers/eequipment.h>

class eTreeColumn {
public:
    void add(eSkillButton* const button) {
        mButtons.emplace_back(button);
    }

    void align() {
        std::sort(mButtons.begin(), mButtons.end(),
                  [](eSkillButton* const b1,
                     eSkillButton* const b2) {
            return b1->y() < b2->y();
        });

        int minY = 0;
        for(const auto b : mButtons) {
            const int y = b->y();
            if(y < minY) {
                b->setY(minY);
            }
            minY = b->y() + 1.5*b->height();
        }
    }
private:
    std::vector<eSkillButton*> mButtons;
};

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

    std::vector<eTreeColumn> columns;
    columns.resize(maxCol + 1);

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
        auto& col = columns[colId];
        col.add(button);
        button->setX(colId*(w - bdim)/maxCol);
        button->setY(skill.fLevelReq*(h - bdim)/skillTree.fMaxLevelReq);
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

    for(auto& col : columns) {
        col.align();
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
        p.drawLine(from, toX, thick, color);
        p.drawLine(toX, to, thick, color);
    }
}
