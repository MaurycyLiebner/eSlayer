#include "eunitindicator.h"

#include "../../elanguage.h"
#include "../../units/eunit.h"
#include "../ecolors.h"

void eUnitIndicator::initialize() {
    mName = new eLabel(window());
    mName->setFontColor(eFontColor::white);
    addWidget(mName);
}

void eUnitIndicator::setUnit(const std::shared_ptr<eUnit>& u) {
    mUnit = u;
    if(mUnit) {
        setRange(0, u->fMaxHealth);
        setValue(u->fHealth);
        const auto name = eLanguage::text(6, mUnit->fTypeId);
        mName->setText(name);
        mName->fitContent();
        mName->show();
        mName->align(eAlignment::center);
    } else {
        mName->hide();
    }
}

void eUnitIndicator::paintEvent(ePainter& p) {
    if(!mUnit) return;
    const double per = double(value() - min())/(max() - min());
    const SDL_Rect baseRect = rect();
    const int pp = padding();
    const SDL_Rect innerRect{pp, pp,
                             baseRect.w - 2*pp,
                             baseRect.h - 2*pp};
    const int w = per*innerRect.w;
    const SDL_Rect rect{innerRect.x, innerRect.y,
                        w, innerRect.h};
    p.fillRect(baseRect, {0, 0, 0, 255});
    p.fillRect(rect, eColors::sHealth);
    p.drawRect(baseRect, {255, 255, 255, 255}, lineWidth());
}
