#include "estatuswidget.h"

#include "../elabel.h"
#include "../../etext.h"
#include "../../names/eservernames.h"
#include "../../names/edifficultynames.h"

#include <eSlayerHelpers/estringhelpers.h>
#include <eSlayerHelpers/edifficulties.h>
#include <eSlayerHelpers/eversion.h>

void eStatusWidget::initialize(
    const eGameSettings& settings) {
    std::vector<eLabel*> labels;

    mAreaName = new eLabel(window());
    mAreaName->setTinyFontSize();
    mAreaName->setTinyPadding();
    mAreaName->setText("Placeholder");
    mAreaName->fitContent();
    addWidget(mAreaName);
    labels.emplace_back(mAreaName);

    const auto sett = new eLabel(window());
    sett->setTinyFontSize();
    sett->setTinyPadding();
    const auto type = settings.fType;
    const auto name = eServerNames::name(type);
    sett->setText(name);
    sett->fitContent();
    addWidget(sett);
    labels.emplace_back(sett);

    const auto& ipStr = settings.fIP;
    if(!ipStr.empty()) {
        const auto ip = new eLabel(window());
        ip->setTinyFontSize();
        ip->setTinyPadding();
        auto fullIpStr = eText::text(14, 0);
        fullIpStr = eStringHelpers::replaceAll(
            fullIpStr, "%1", ipStr);
        ip->setText(fullIpStr);
        ip->fitContent();
        addWidget(ip);
        labels.emplace_back(ip);
    }

    const auto& passStr = settings.fPassword;
    if(!passStr.empty()) {
        const auto pass = new eLabel(window());
        pass->setTinyFontSize();
        pass->setTinyPadding();
        auto fullPassStr = eText::text(14, 1);
        fullPassStr = eStringHelpers::replaceAll(
            fullPassStr, "%1", passStr);
        pass->setText(fullPassStr);
        pass->fitContent();
        addWidget(pass);
        labels.emplace_back(pass);
    }

    const auto diffLabel = new eLabel(window());
    diffLabel->setTinyFontSize();
    diffLabel->setTinyPadding();
    const auto diff = eDifficulties::sDifficulty;
    const auto diffName = eDifficultyNames::name(diff);
    auto diffStr = eText::text(14, 2);
    diffStr = eStringHelpers::replaceAll(diffStr, "%1", diffName);
    diffLabel->setText(diffStr);
    diffLabel->fitContent();
    addWidget(diffLabel);
    labels.emplace_back(diffLabel);

    const auto v = new eLabel(window());
    v->setTinyFontSize();
    v->setTinyPadding();
    v->setText("v " + eVersion);
    v->fitContent();
    addWidget(v);
    labels.emplace_back(v);

    stackVertically();
    fitContent();
    for(const auto l : labels) {
        l->align(eAlignment::right);
    }
}

void eStatusWidget::setAreaName(
    const std::string& name) {
    mAreaName->setText(name);
    mAreaName->fitContent();
    mAreaName->align(eAlignment::right);
}
