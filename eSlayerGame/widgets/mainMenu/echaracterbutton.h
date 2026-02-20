#ifndef ECHARACTERBUTTON_H
#define ECHARACTERBUTTON_H

#include "../echeckbutton.h"

#include "../../textures/echarunitmodel.h"

class eCharacter;

class eCharacterButton : public eCheckButton {
public:
    using eCheckButton::eCheckButton;

    void initialize(const eCharacter& c);

    const std::string& charName() const { return mCharName; }
protected:
    void paintEvent(ePainter& p) override;
private:
    std::string mCharName;
    eCharUnitModel mModel;
    int mFrame = 0;
};

#endif // ECHARACTERBUTTON_H
