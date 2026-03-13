#ifndef EPLAYERHEALTHINDICATOR_H
#define EPLAYERHEALTHINDICATOR_H

#include "ehealthindicator.h"

class ePlayerHealthIndicator : public eHealthIndicator {
public:
    using eHealthIndicator::eHealthIndicator;

    void setName(const std::string& name);
protected:
    void paintEvent(ePainter& p) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
    bool mouseEnterEvent(const eMouseEvent& e) override;
    bool mouseLeaveEvent(const eMouseEvent& e) override;
private:
    std::string mName;
    bool mHovered = false;
};

#endif // EPLAYERHEALTHINDICATOR_H
