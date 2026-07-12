#include "ehirewidget.h"

#include "ewaypointline.h"
#include "../../etext.h"
#include "../../names/emercenarynames.h"
#include "../escrollwidget.h"

#include <eSlayerHelpers/estringhelpers.h>

void eHireWidget::initialize(
    const std::vector<eHireInfo>& info,
    const uint32_t gold,
    const eHireAction& hireAction,
    const eAction& closeAction) {
    auto goldText = eText::text(17, 1);
    const auto goldStr = std::to_string(gold);
    goldText = eStringHelpers::replaceAll(goldText, "%1", goldStr);

    const auto& res = resolution();
    const int p = res.largePadding();
    const float mult = res.multiplier();

    const int singleW = 800*mult;
    const int singleH = 50*mult;
    const int totalH = 8*singleH;

    const auto title = eText::text(17, 2);

    const auto innerW = new eWidget(window());
    innerW->setNoPadding();

    const auto titleW = new eWidget(window());
    titleW->setNoPadding();

    const auto goldLabel = new eLabel(window());
    goldLabel->setNoPadding();
    goldLabel->setSmallFontSize();
    goldLabel->setText(goldText);
    goldLabel->fitContent();
    titleW->addWidget(goldLabel);

    const auto titleLabel = new eLabel(window());
    titleLabel->setNoPadding();
    titleLabel->setSmallFontSize();
    titleLabel->setText(title);
    titleLabel->fitContent();
    titleW->addWidget(titleLabel);

    titleW->fitContent();
    titleW->setWidth(singleW);
    titleW->layoutHorizontallyWithoutSpaces();

    innerW->addWidget(titleW);

    const auto scroll = new eScrollWidget(window());
    scroll->resize(singleW, totalH);

    const auto linesW = new eWidget(window());
    linesW->setNoPadding();
    for(const auto& i : info) {
        const auto line = new eWaypointLine(window());
        eAction action;
        if(gold >= i.fCost) {
            action = [hireAction, i, closeAction]() {
                hireAction(i);
                closeAction();
            };
        }
        line->resize(singleW, singleH);
        line->setWrapWidth(singleW);
        const auto& names = eMercenaryNames::sNames.get(i.fMercType);
        const auto& name = names[i.fNameId % names.size()];
        auto lineText = eText::text(17, 4);
        lineText = eStringHelpers::replaceAll(lineText, "%1", name);
        lineText = eStringHelpers::replaceAll(lineText, "%2", i.fLevel);
        lineText = eStringHelpers::replaceAll(lineText, "%3", i.fCost);
        const auto& mercTypeName = eMercenaryNames::sTypeNames[i.fMercType];
        lineText = eStringHelpers::replaceAll(lineText, "%4", mercTypeName);
        line->initialize(lineText, action, false);
        line->fitHeight();
        linesW->addWidget(line);
    }
    linesW->stackVertically(p);
    linesW->fitContent();
    scroll->setScrollArea(linesW);

    innerW->addWidget(scroll);

    {
        const auto line = new eWaypointLine(window());
        line->resize(singleW, singleH);
        const auto name = eText::text(17, 3);
        line->initialize(name, closeAction, false);
        innerW->addWidget(line);
    }

    innerW->stackVertically(p, true);
    innerW->fitContent();

    setup(innerW);
}