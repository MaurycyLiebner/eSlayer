#include "eunitindicator.h"

#include "../../units/eunit.h"
#include "../ecolors.h"
#include "../../names/emonsternames.h"
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
                             const std::map<int, std::string>& names) {
    mUnit = u;
    std::string modsStr;
    if(u) {
        const auto it = names.find(u->fCharId);
        if(it == names.end()) {
            const int typeId = u->fCharDataId;
            const auto name = eMonsterNames::name(typeId);
            setText(name);
        } else {
            setText(it->second);
        }
        const auto& mods = u->fMods;
        for(const auto m : mods) {
            if(!modsStr.empty()) modsStr += ", ";
            modsStr += eEliteModifiersNames::name(m);
        }
    } else {
        setText("");
    }
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
