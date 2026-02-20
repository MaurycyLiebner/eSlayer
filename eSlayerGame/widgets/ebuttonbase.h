#ifndef EBUTTONBASE_H
#define EBUTTONBASE_H

#include "elabel.h"

class eButtonBase : public eLabel {
public:
    using eLabel::eLabel;

    void setPressAction(const eAction& a);
    void setRightPressAction(const eAction& a);
    void setMouseEnterAction(const eAction& a);
    void setMouseLeaveAction(const eAction& a);

    void setEnabled(const bool b);
    bool enabled() const;

    bool pressed() const;
    bool hovered() const;

    void trigger() const;
protected:
    bool mousePressEvent(const eMouseEvent& e) override;
    bool mouseReleaseEvent(const eMouseEvent& e) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
    bool mouseEnterEvent(const eMouseEvent& e) override;
    bool mouseLeaveEvent(const eMouseEvent& e) override;
private:
    eAction mPressAction;
    eAction mRightPressAction;
    eAction mEnterAction;
    eAction mLeaveAction;

    bool mEnabled = true;
    bool mPressed = false;
    bool mHover = false;
};

#endif // EBUTTONBASE_H
