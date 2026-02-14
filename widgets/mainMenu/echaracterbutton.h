#ifndef ECHARACTERBUTTON_H
#define ECHARACTERBUTTON_H

#include "../echeckablebutton.h"

class eCharacter;

class eCharacterButton : public eCheckableButton {
public:
    using eCheckableButton::eCheckableButton;

    void initialize(const eCharacter& c);
protected:
    void paintEvent(ePainter& p) override;
};

#endif // ECHARACTERBUTTON_H
