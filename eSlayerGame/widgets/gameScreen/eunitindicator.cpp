#include "eunitindicator.h"

#include "../ecolors.h"
#include "../../units/eunit.h"
#include "../../names/eelitemodifiersnames.h"
#include "../../etext.h"

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
    mModsLabel->setText("placeholder");
    mModsLabel->fitContent();
    mModsLabel->setText("");

    mImmunitiesW = new eWidget(window());
    mImmunitiesW->setNoPadding();
    mImmunitiesW->setSizeHintSkipHidden(true);
    const int b = mModsLabel->y() + mModsLabel->height();
    addWidget(mImmunitiesW);
    mImmunitiesW->setY(b + p);

    for(const auto i : {eUnitData::ifire,
                        eUnitData::icold,
                        eUnitData::ilightning,
                        eUnitData::ipoison,
                        eUnitData::iphysical}) {
        const auto text = eText::text(10, i);
        const auto label = new eLabel(window());
        label->setNoPadding();
        label->setTinyFontSize();
        label->setText(text);
        label->fitContent();

        eFontColor color;
        switch(i) {
        case eUnitData::ifire:
            color = eFontColor::fire;
            break;
        case eUnitData::icold:
            color = eFontColor::cold;
            break;
        case eUnitData::ilightning:
            color = eFontColor::lightning;
            break;
        case eUnitData::ipoison:
            color = eFontColor::poison;
            break;
        case eUnitData::iphysical:
        default:
            color = eFontColor::physical;
            break;
        }

        label->setFontColor(color);

        mImmunityLabels[i] = label;

        mImmunitiesW->addWidget(label);
    }
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

    for(const auto it : mImmunityLabels) {
        const auto imm = it.first;
        const auto label = it.second;
        const bool r = u ? u->getImmunity(imm) : false;
        label->setVisible(r);
    }

    const auto& res = resolution();
    const int p = res.smallPadding();

    mImmunitiesW->stackHorizontally(p, true);
    mImmunitiesW->fitContent();
    mImmunitiesW->align(eAlignment::hcenter);
}

void eUnitIndicator::paintEvent(ePainter& p) {
    if(const auto u = mUnit.lock()) {
        setRange(0, u->fMaxHealth);
        setValue(u->fHealth);
        eHealthIndicator::paintEvent(p);
    }
}
