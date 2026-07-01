#ifndef ESCROLLWIDGET_H
#define ESCROLLWIDGET_H

#include "ewidget.h"

class eScrollWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initializeButtons();

    void setScrollArea(eWidget* const w);
    eWidget* scrollArea() const { return mScrollArea; }

    void scrollUp();
    void scrollDown();
    void setDY(const int dy);
    void scrollToTheTop();
    void scrollToTheBottom();
    void clampDY();
    void setScrollStep(const int s);

    int dy() const { return mDy; }
protected:
    void paintEvent(ePainter& p) override;

    bool keyPressEvent(const eKeyPressEvent& e) override;
    bool mouseWheelEvent(const eMouseWheelEvent& e) override;
private:
    int mStep = 10;
    int mDy = 0;

    eWidget* mScrollArea = nullptr;
};

#endif // ESCROLLWIDGET_H
