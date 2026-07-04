#include "emessageswidget.h"

#include "../escrollwidget.h"
#include "../elabel.h"

void eMessagesWidget::initialize(
    const std::vector<std::string>& log) {
    mLog = &log;

    const auto& res = resolution();
    const float mult = res.multiplier();
    const int w = mult*600;
    const int h = mult*500;

    mContainer = new eWidget(window());
    mContainer->setNoPadding();
    mContainer->resize(w, h);

    const auto sw = new eScrollWidget(window());
    sw->resize(w, h);
    sw->setScrollArea(mContainer);
    setup(sw);

    updateText();
}

void eMessagesWidget::paintEvent(ePainter& p) {
    eBgWidget::paintEvent(p);
    updateText();
}

void eMessagesWidget::updateText() {
    const auto& logRef = *mLog;
    const auto logSize = logRef.size();
    if(logSize == mLatestCount) return;
    const int w = mContainer->width();
    for(int i = mLatestCount; i < logSize; i++) {
        const auto& m = logRef[i];
        const auto mw = new eLabel(window());
        mw->setNoPadding();
        mw->setSmallFontSize();
        mw->setWrapWidth(w);
        mw->setText(m);
        mw->fitContent();
        mContainer->prependWidget(mw);
    }
    const auto& res = resolution();
    const int pp = res.smallPadding();
    mContainer->stackVertically(pp);
    mContainer->fitHeight();
    mLatestCount = logRef.size();
}
