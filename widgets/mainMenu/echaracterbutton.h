#ifndef ECHARACTERBUTTON_H
#define ECHARACTERBUTTON_H

#include "../echeckbutton.h"

class eCharacter;

class eCharacterButton : public eCheckButton {
public:
    using eCheckButton::eCheckButton;

    void initialize(const eCharacter& c);
};

#endif // ECHARACTERBUTTON_H
