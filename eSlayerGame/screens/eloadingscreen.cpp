#include "eloadingscreen.h"

#include "../widgets/eprogressbar.h"

void eLoadingScreen::initialize(
    const std::vector<eAction>& loading,
    const eAction& finish) {
    const auto inner = eScreenBase::addInner();

    const auto& res = resolution();
    const int width = inner->width();
    const int height = 4*res.largePadding();

    const auto pb = new eProgressBar(window());
    pb->resize(width, height);
    pb->setRange(0, loading.size());
    inner->addWidget(pb);
    pb->align(eAlignment::center);

    for(const auto& l : loading) {
        mLoading.push([l, pb]() {
            pb->setValue(pb->value() + 1);
            l();
        });
    }
    mLoading.push(finish);
}

void eLoadingScreen::paintEvent(ePainter& p) {
    eScreenBase::paintEvent(p);
    if(!mLoading.empty()) {
        const auto l = mLoading.front();
        mLoading.pop();
        l();
    }
}
