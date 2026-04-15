#ifndef EBGWIDGET_H
#define EBGWIDGET_H

#include "../ewidget.h"

class eBgWidget : public eWidget {
public:
    using eWidget::eWidget;
protected:
    void paintEvent(ePainter& p) override;
};

#endif // EBGWIDGET_H
