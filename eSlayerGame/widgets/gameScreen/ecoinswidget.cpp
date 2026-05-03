#include "ecoinswidget.h"

#include "../elabel.h"
#include "../ebuttonbase.h"
#include "../../textures/euitextures.h"

void eCoinsWidget::initialize(const int count) {
    setNoPadding();

    const auto coinsButton = new eButtonBase(window());
    coinsButton->setTexture(eUITextures::sCoins);
    coinsButton->setNoPadding();
    coinsButton->fitContent();
    coinsButton->setTooltip("Drop Gold");
    addWidget(coinsButton);

    mLabel = new eLabel(window());
    mLabel->setSmallFontSize();
    mLabel->setText(std::to_string(count));
    mLabel->fitWidth();
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
