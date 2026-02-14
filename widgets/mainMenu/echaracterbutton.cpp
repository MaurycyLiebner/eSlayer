#include "echaracterbutton.h"

#include "../../echaracter.h"

void eCharacterButton::initialize(const eCharacter& c) {
    setText(c.name());
    fitContent();
}
