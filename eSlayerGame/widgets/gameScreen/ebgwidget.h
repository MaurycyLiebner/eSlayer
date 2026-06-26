#ifndef EBGWIDGET_H
#define EBGWIDGET_H

#include "../ewidget.h"

class eBgWidget : public eWidget {
public:
    using eWidget::eWidget;
protected:
    void setup(eWidget* const inner);

    void paintEvent(ePainter& p) override;
    bool mousePressEvent(const eMouseEvent& e) override;
};

#endif // EBGWIDGET_H
