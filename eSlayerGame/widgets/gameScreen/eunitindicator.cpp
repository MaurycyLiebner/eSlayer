#include "eunitindicator.h"

#include "../../elanguage.h"
#include "../../units/eunit.h"
#include "../ecolors.h"

void eUnitIndicator::initialize() {
    eHealthIndicator::initialize();
    setColor(eColors::sHealth);
}

void eUnitIndicator::setUnit(const std::shared_ptr<eUnit>& u) {
    mUnit = u;
    if(mUnit) {
        const auto name = eLanguage::text(6, mUnit->fTypeId);
        setText(name);
    } else {
        setText("");
    }
}

void eUnitIndicator::paintEvent(ePainter& p) {
    if(!mUnit) return;
    setRange(0, mUnit->fMaxHealth);
    setValue(mUnit->fHealth);
    eHealthIndicator::paintEvent(p);
}
