#include "eskilltreeswidget.h"

#include "eskilltreewidget.h"
#include "../ebuttonbase.h"
#include "../../names/eskilltreenames.h"
#include "../elabel.h"
#include "../../etext.h"

#include <eSlayerHelpers/estats.h>
#include <eSlayerHelpers/eclasses.h>
#include <eSlayerHelpers/eskilltrees.h>

class eTreeTabButton : public eButtonBase {
public:
    using eButtonBase::eButtonBase;

    void paintEvent(ePainter& p) {
        if(enabled()) {
            setFontColor(eFontColor::white);
            if(hovered()) {
                const SDL_Color white{255, 255, 255, 255};
                p.drawRect(rect(), white, lineWidth());
            }
        } else {
            setFontColor(eFontColor::gray);
        }
        return eButtonBase::paintEvent(p);
    }
};

void eSkillTreesWidget::initialize(
    eStats& stats, const eAttributes& attrs,
    const eEquipment& eq) {
    mStats = &stats;

    const auto inner = new eWidget(window());
    inner->setNoPadding();

    const auto tabWidget = new eWidget(window());
    tabWidget->setNoPadding();

    const auto& res = resolution();
    const float mult = res.multiplier();
    const int tabWidth = 100*mult;
    tabWidget->setWidth(tabWidth);

    const auto remainingLabel = new eLabel(window());
    remainingLabel->setNoPadding();
    remainingLabel->setWrapWidth(tabWidth);
    remainingLabel->setTextAlignment(eAlignment::center);
    remainingLabel->setSmallFontSize();
    remainingLabel->setText(eText::text(13, 0));
    remainingLabel->fitContent();
    tabWidget->addWidget(remainingLabel);
    remainingLabel->align(eAlignment::hcenter);

    mRemPtsLabel = new eLabel(window());
    mRemPtsLabel->setNoPadding();
    mRemPtsLabel->setSmallFontSize();
    const auto& levels = stats.fBaseSkillLevels;
    const int pts = levels.fRemainingPoints;
    mRemPtsLabel->setText(std::to_string(pts));
    mRemPtsLabel->fitContent();
    tabWidget->addWidget(mRemPtsLabel);
    mRemPtsLabel->align(eAlignment::hcenter);

    const int p = res.hugePadding();

    const auto skillTreeWidget = new eWidget(window());
    skillTreeWidget->setNoPadding();

    const auto& class_ = eClasses::sClasses.get(stats.fClass);
    for(const int skillTreeId : class_.fSkillTrees) {
        const auto skillTreeW = new eSkillTreeWidget(window());
        skillTreeW->initialize(skillTreeId, stats);
        skillTreeWidget->addWidget(skillTreeW);
        skillTreeW->setVisible(mWidgets.empty());
        mWidgets.emplace_back(skillTreeW);

        const auto skillTreeName = eSkillTreeNames::name(skillTreeId);

        const auto tabB = new eTreeTabButton(window());
        tabB->setSmallFontSize();
        tabB->setWrapWidth(tabWidth);
        tabB->setText(skillTreeName);
        tabB->fitContent();
        tabWidget->addWidget(tabB);
        tabB->align(eAlignment::hcenter);
        tabB->setPressAction([this, skillTreeW]() {
            for(const auto w : mWidgets) {
                w->hide();
            }
            skillTreeW->show();
        });
    }

    tabWidget->stackVertically(p);
    tabWidget->fitHeight();
    skillTreeWidget->fitContent();
    for(const auto w : mWidgets) {
        w->align(eAlignment::center);
    }
    inner->addWidget(skillTreeWidget);
    inner->addWidget(tabWidget);
    inner->stackHorizontally(p);
    inner->fitContent();
    setup(inner);
}

void eSkillTreesWidget::paintEvent(ePainter& p) {
    const auto& levels = mStats->fBaseSkillLevels;
    const int pts = levels.fRemainingPoints;
    mRemPtsLabel->setText(std::to_string(pts));
    mRemPtsLabel->fitContent();
    mRemPtsLabel->align(eAlignment::hcenter);
    eBgWidget::paintEvent(p);
}