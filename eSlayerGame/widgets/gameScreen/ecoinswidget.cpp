#include "ecoinswidget.h"

#include "../elabel.h"
#include "../ebuttonbase.h"
#include "../../etext.h"
#include "../../textures/euitextures.h"

#include "einventorywidget.h"
#include "estashwidget.h"

void eCoinsWidget::initialize(
    const int count,
    const eAction& action,
    const int tooltipS) {
    setNoPadding();

    const auto coinsButton = new eButtonBase(window());
    coinsButton->setTexture(eUITextures::sCoins);
    coinsButton->setNoPadding();
    coinsButton->fitContent();
    coinsButton->setTooltip(eText::text(18, tooltipS));
    addWidget(coinsButton);
    coinsButton->setPressAction(action);

    mLabel = new eLabel(window());
    mLabel->setSmallFontSize();
    const int h = coinsButton->height();
    mLabel->setHeight(h);
    setCount(count);
    addWidget(mLabel);

    stackHorizontally();
    fitContent();
}

void eCoinsWidget::setCount(const int count) {
    mLabel->setText(std::to_string(count));
    mLabel->fitWidth();
}
