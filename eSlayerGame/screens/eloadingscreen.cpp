#include "eloadingscreen.h"

#include "../emainwindow.h"
#include "../widgets/eprogressbar.h"

void eLoadingScreen::initialize(
    const std::vector<eAction>& loading,
    const eAction& finish) {
    const auto inner = eScreenBase::addInner();

    const auto res = resolution();
    const int width = inner->width();
    const int height = 4*res.largePadding();

    const auto w = window();

    const auto pb = new eProgressBar(window());
    pb->resize(width, height);
    pb->setRange(0, loading.size());
    inner->addWidget(pb);
    pb->align(eAlignment::center);

    w->addLoad(finish);
    for(const auto& l : loading) {
        w->addLoad([l, pb]() {
            pb->setValue(pb->value() + 1);
            l();
        });
    }
}
