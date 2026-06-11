#include "eunitindicator.h"

#include "../../units/eunit.h"
#include "../ecolors.h"
#include "../../names/eelitemodifiersnames.h"

void eUnitIndicator::initialize() {
    eHealthIndicator::initialize();
    setColor(eColors::sHealth);

    mModsLabel = new eLabel(window());
    mModsLabel->setFontColor(eFontColor::white);
    mModsLabel->setNoPadding();
    mModsLabel->setTinyFontSize();
    addWidget(mModsLabel);
    const auto& res = resolution();
    const int h = height();
    const int p = res.largePadding();
    mModsLabel->setY(h + p);
}

void eUnitIndicator::setUnit(const std::shared_ptr<eUnit>& u,
                             const std::string& name) {
    mUnit = u;
    std::string modsStr;
    if(u) {
        const auto& mods = u->fMods;
        for(const auto m : mods) {
            if(!modsStr.empty()) modsStr += ", ";
            modsStr += eEliteModifiersNames::name(m);
        }
    }
    setText(name);
    mModsLabel->setText(modsStr);
    mModsLabel->fitContent();
    mModsLabel->align(eAlignment::hcenter);
}

void eUnitIndicator::paintEvent(ePainter& p) {
    if(const auto u = mUnit.lock()) {
        setRange(0, u->fMaxHealth);
        setValue(u->fHealth);
        eHealthIndicator::paintEvent(p);
    }
}
