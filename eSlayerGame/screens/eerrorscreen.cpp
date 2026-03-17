#include "eerrorscreen.h"

#include "../widgets/elabel.h"
#include "../elanguage.h"

void eErrorScreen::initialize(
    const std::string& msg,
    const std::string& subMsg,
    const eAction& finish) {
    mFinish = finish;

    const auto w = window();

    const auto inner = eScreenBase::addInner();

    const auto res = resolution();

    const auto msgLabel = new eLabel(w);
    const int width = inner->width();
    msgLabel->setWrapWidth(width);
    msgLabel->setHugeFontSize();
    msgLabel->setFontColor(eFontColor::white);
    msgLabel->setText(msg);
    msgLabel->fitContent();
    inner->addWidget(msgLabel);

    const auto subMsgLabel = new eLabel(w);
    subMsgLabel->setWrapWidth(width);
    subMsgLabel->setLargeFontSize();
    subMsgLabel->setFontColor(eFontColor::white);
    subMsgLabel->setText(subMsg);
    subMsgLabel->fitContent();
    inner->addWidget(subMsgLabel);

    const auto escLabel = new eLabel(w);
    escLabel->setHugeFontSize();
    escLabel->setFontColor(eFontColor::gray);
    escLabel->setText(eLanguage::text(9, 0));
    escLabel->fitContent();
    inner->addWidget(escLabel);

    inner->stackVertically();

    msgLabel->align(eAlignment::hcenter);
    subMsgLabel->align(eAlignment::hcenter);
    escLabel->align(eAlignment::hcenter | eAlignment::bottom);
}

bool eErrorScreen::keyPressEvent(const eKeyPressEvent& e) {
    if(e.key() == SDL_SCANCODE_ESCAPE) {
        mFinish();
    }
    return true;
}
