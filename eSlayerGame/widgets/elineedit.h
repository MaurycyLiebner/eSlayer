#ifndef ELINEEDIT_H
#define ELINEEDIT_H

#include "elabel.h"

class eLineEdit : public eLabel {
public:
    eLineEdit(eMainWindow* const window);

    void setChangeAction(const eAction a);

    void setMaxLength(const int max);
    void setMaxLengthAndFit(const int max);

    void allow(const char c);
    void disallow(const char c);

    void setAllowed(const std::string& a);
protected:
    bool mouseMoveEvent(const eMouseEvent& e) override;
    bool mouseEnterEvent(const eMouseEvent& e) override;
    bool mouseLeaveEvent(const eMouseEvent& e) override;
    bool keyPressEvent(const eKeyPressEvent& e) override;
    bool textInputEvent(const eTextInputEvent& e) override;
    void paintEvent(ePainter& p) override;
    void sizeHint(int& w, int& h) override;
private:
    bool mHovered = false;
    bool mRenderBg = false;
    eAction mChangeAction;
    int mMaxLength = 15;
    int mTime = 0;
    std::string mAllowed = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM-_1234567890";
};

#endif // ELINEEDIT_H
