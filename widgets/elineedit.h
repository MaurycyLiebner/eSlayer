#ifndef ELINEEDIT_H
#define ELINEEDIT_H

#include "elabel.h"

class eLineEdit : public eLabel {
public:
    eLineEdit(eMainWindow* const window);

    void setChangeAction(const eAction a);

    void setMaxLength(const int max);
protected:
    bool mouseMoveEvent(const eMouseEvent& e) override;
    bool mouseEnterEvent(const eMouseEvent& e) override;
    bool mouseLeaveEvent(const eMouseEvent& e) override;
    bool keyPressEvent(const eKeyPressEvent& e) override;
    void paintEvent(ePainter& p) override;
    void sizeHint(int& w, int& h) override;
private:
    bool mHovered = false;
    bool mRenderBg = false;
    eAction mChangeAction;
    int mMaxLength = 15;
    int mTime = 0;
};

#endif // ELINEEDIT_H
