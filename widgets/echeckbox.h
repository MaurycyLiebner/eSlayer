#ifndef ECHECKBOX_H
#define ECHECKBOX_H

#include "echeckablebutton.h"

class eCheckBox : public eCheckableButton {
public:
    using eCheckableButton::eCheckableButton;
protected:
    void sizeHint(int& w, int& h) override;
    void paintEvent(ePainter& p) override;
};

#endif // ECHECKBOX_H
