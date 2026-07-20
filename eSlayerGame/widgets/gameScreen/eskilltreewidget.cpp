#include "eskilltreewidget.h"

#include "eskillbutton.h"
#include "egamewidget.h"
#include "../../textures/etexturecolorholder.h"

#include <eSlayerHelpers/eskilltrees.h>
#include <eSlayerHelpers/estats.h>
#include <eSlayerHelpers/eattributes.h>
#include <eSlayerHelpers/eequipment.h>

class eSkillTreeButton : public eSkillButton {
public:
    using eSkillButton::eSkillButton;

    void setLevel(const int level) {
        if(level >= 0) {
            setBottomRightText(std::to_string(level + 1));
        } else {
            setBottomRightText("");
        }
    }

    void initialize(const eSkillTreeSkill& skill,
                    const eStats& stats) {
        eSkillButton::initialize();
        const int skillId = skill.fSkillId;
        setSkillId(skillId);
        mLevelReq = skill.fLevelReq;
        mPrerequisites = skill.fPrerequisites;
        setLevelReq(mLevelReq);
        setLevelReqMet(stats.fLevel >= mLevelReq);
        mHasPoints = stats.skillPoints() > 0;
    }

    void updateEnabled(const eStats& stats) {
        mHasPoints = stats.skillPoints() > 0;
        const int skillId = eSkillButton::skillId();
        const int level = stats.baseSkillLevel(skillId);
        if(level > 0) return setEnabled(true);
        if(stats.fLevel < mLevelReq) return setEnabled(false);
        for(const int p : mPrerequisites) {
            const int level = stats.baseSkillLevel(p);
            if(level < 0) return setEnabled(false);
        }
        setEnabled(mHasPoints);
    }
protected:
    void paintEvent(ePainter& p) override {
        const auto& tex = texture();
        if(!tex) return;
        const bool e = enabled();
        eTextureColorSetting color;
        if(e) {
            color.set(1.f, 1.f, 1.f, 1.f);
        } else {
            color.set(0.5f, 0.5f, 0.5f, 1.f);
        }
        const eTextureColorHolder h(color, tex);
        p.drawTexture(0, 0, tex);
        const bool hovered = eWidget::hovered();
        if(mHasPoints && e && hovered) {
            tex->setBlendMode(SDL_BLENDMODE_ADD);
            tex->setAlpha(128);
            p.drawTexture(0, 0, tex);
            tex->setBlendMode(SDL_BLENDMODE_BLEND);
            tex->clearAlphaMod();
        }
    }
private:
    bool mHasPoints = false;
    int mLevelReq = 1;
    std::set<int> mPrerequisites;
};

void eSkillTreeWidget::initialize(
    const int skillTreeId, eStats& stats) {
    mStats = &stats;

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

    std::map<int, eSkillTreeButton*> skillMap;

    for(const auto& skill : skillTree.fSkills) {
        const int skillId = skill.fSkillId;
        const auto button = new eSkillTreeButton(window());
        skillMap[skillId] = button;
        button->initialize(skill, stats);
        mButtons.emplace_back(button);
        const int level = stats.effectiveSkillLevel(skillId);
        button->setLevel(level);
        button->updateEnabled(stats);
        button->setPressAction([this, &stats, &skill, button, skillId]() {
            const bool e = button->enabled();
            if(!e) return;
            const int level = stats.incSkillLevel(skillId);
            button->setLevel(level);
            eGameWidget::sSendSkillLevelsChanged();
            updateEnabled();
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

void eSkillTreeWidget::updateEnabled() {
    for(const auto b : mButtons) {
        b->updateEnabled(*mStats);
    }
}
