#include "escreenbase.h"

void eScreenBase::setExit(const eAction& exitA) {
    mExit = exitA;
}

eWidget* eScreenBase::addInner() {
    const auto frame = new eWidget(window());
    addWidget(frame);

    const auto res = resolution();

    const int p = res.largePadding();
    const int cww = res.centralWidgetLargeWidth();
    const int cwh = res.centralWidgetLargeHeight();
    frame->resize(cww, cwh);

    frame->align(eAlignment::center);

    const auto inner = new eWidget(window());
    inner->setNoPadding();
    frame->addWidget(inner);
    inner->move(2*p, 2*p);
    inner->resize(cww - 4*p, cwh - 4*p);

    return inner;
}

bool eScreenBase::keyPressEvent(const eKeyPressEvent& e) {
    if(e.key() == SDL_SCANCODE_ESCAPE) {
        if(mExit) {
            mExit();
            return true;
        }
    }
    return false;
}
