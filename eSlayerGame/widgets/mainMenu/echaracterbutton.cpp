#include "echaracterbutton.h"

#include "../../echaracter.h"
#include "../../textures/echarstextures.h"

void eCharacterButton::initialize(const eCharacter& c) {
    mCharName = c.name();
    // const eCharTextures::eModelParts modelParts {
    //     {"hd", "bare"},
    //     {"la", "bare"},
    //     {"lg", "bare"},
    //     {"ra", "bare"},
    //     {"tr", "bare"}
    // };
    // const auto texs = eCharsTextures::get("char");
    const eCharTextures::eModelParts modelParts {
        {"whole", "light"}
    };
    const auto texs = eCharsTextures::get("pal");
    const auto r = renderer();
    const auto model = texs->generateModel(modelParts, r);
    mModel.setCharModel(model);
    mModel.setAnimation(0, 1.);
    mModel.setDirection(0);

    const auto label = new eLabel(c.name(), window());
    addWidget(label);
    label->fitContent();
    label->align(eAlignment::vcenter);
    label->setX(width()/3);
}

void eCharacterButton::paintEvent(ePainter& p) {
    p.save();
    p.translate(width()/5, 3*height()/4);
    mModel.incFrame(1.);
    mModel.drawBase(p);
    p.restore();
    eCheckButton::paintEvent(p);
    mFrame++;
}
