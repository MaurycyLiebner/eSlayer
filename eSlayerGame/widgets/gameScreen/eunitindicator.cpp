#include "eunitindicator.h"

#include "../../units/eunit.h"
#include "../ecolors.h"
#include "../../names/emonsternames.h"

void eUnitIndicator::initialize() {
    eHealthIndicator::initialize();
    setColor(eColors::sHealth);
}

void eUnitIndicator::setUnit(const std::shared_ptr<eUnit>& u,
                             const std::map<int, std::string>& names) {
    mUnit = u;
    if(mUnit) {
        const auto it = names.find(u->fCharId);
        if(it == names.end()) {
            const int typeId = mUnit->fCharDataId;
            const auto name = eMonsterNames::name(typeId);
            setText(name);
        } else {
            setText(it->second);
        }
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
