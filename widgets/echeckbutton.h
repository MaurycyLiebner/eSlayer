#ifndef ECHECKBUTTON_H
#define ECHECKBUTTON_H

#include "echeckablebutton.h"

class eCheckButton : public eCheckableButton {
public:
    using eCheckableButton::eCheckableButton;
protected:
    void paintEvent(ePainter& p) override;
};

#endif // ECHECKBUTTON_H
