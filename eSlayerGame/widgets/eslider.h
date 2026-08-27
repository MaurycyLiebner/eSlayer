#ifndef ESLIDER_H
#define ESLIDER_H

#include "eprogressbar.h"

using eSetter = std::function<void(int)>;

class eSlider : public eProgressBar {
public:
    using eProgressBar::eProgressBar;

    void setSetter(const eSetter& setter);
protected:
    bool mousePressEvent(const eMouseEvent& e) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
private:
    void triggerX(const int x);

    eSetter mSetter;
};

#endif // ESLIDER_H
