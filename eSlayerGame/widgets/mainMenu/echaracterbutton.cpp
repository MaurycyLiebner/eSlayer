#include "echaracterbutton.h"

#include "../../textures/echarstextures.h"

#include <eSlayerHelpers/echaracter.h>
#include <eSlayerHelpers/eclasses.h>
#include <eSlayerHelpers/eunitsinfo.h>

void eCharacterButton::initialize(const eCharacter& c) {
    const auto ready = std::make_shared<bool>(false);
    mReady = ready;

    mCharName = c.name();
    const auto& eq = c.equipment();
    const auto partsMap = eq.partsMap();
    const int classId = c.classId();
    const auto& class_ = eClasses::sClasses.get(classId);
    const auto uinfoId = class_.fUnitInfoId;
    const auto& uinfo = eUnitsInfo::sUnits.get(uinfoId);
    const auto& data = eCharsTextures::get(uinfo.fCharData);
    const auto modelParts = data.mapToModelParts(partsMap);
    const auto& res = resolution();
    const auto r = renderer();
    const auto finished = [ready](const std::shared_ptr<eCharModel>&) {
        *ready = true;
    };
    const auto model = data.requestModel(
        modelParts, res, r, finished, true);
    mModel.setCharModel(model);
    mModel.setAnimation(0, 1.f);
    mModel.setDirection(0);

    const auto label = new eLabel(c.name(), window());
    addWidget(label);
    label->fitContent();
    label->align(eAlignment::vcenter);
    label->setX(width()/3);
}

void eCharacterButton::paintEvent(ePainter& p) {
    if(*mReady) {
        p.save();
        p.translate(width()/5, 3*height()/4);
        mModel.incFrame(1.f);
        const auto& res = resolution();
        mModel.draw(p);
        p.restore();
        mFrame++;
    }
    eCheckButton::paintEvent(p);
}
