#include "echeckablebutton.h"

eCheckableButton::eCheckableButton(eMainWindow* const window) :
    eButtonBase(window) {
    setPressAction([this]() {
        mChecked = !mChecked;
    });
}

void eCheckableButton::setCheckAction(const eCheckAction &a) {
    setPressAction([this, a]() {
        mChecked = !mChecked;
        a(mChecked);
    });
}
