#ifndef EPLAYERHEALTHINDICATOR_H
#define EPLAYERHEALTHINDICATOR_H

#include "ehealthindicator.h"

class ePlayerHealthIndicator : public eHealthIndicator {
public:
    using eHealthIndicator::eHealthIndicator;

    void initialize(const std::shared_ptr<eTexture>& bg,
                    const std::shared_ptr<eTexture>& fg,
                    const int nColumns);

    void setName(const std::string& name);
protected:
    void paintEvent(ePainter& p) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
    bool mouseEnterEvent(const eMouseEvent& e) override;
    bool mouseLeaveEvent(const eMouseEvent& e) override;
    bool mousePressEvent(const eMouseEvent& e) override;
private:
    void updateTextVisibility();

    std::string mName;
    bool mHovered = false;
    bool mShowText = false;

    std::shared_ptr<eTexture> mBg;
    std::shared_ptr<eTexture> mFg;
};

#endif // EPLAYERHEALTHINDICATOR_H
