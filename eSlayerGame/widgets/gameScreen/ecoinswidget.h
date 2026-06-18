#ifndef ECOINSWIDGET_H
#define ECOINSWIDGET_H

#include "../ewidget.h"

class eLabel;

class eCoinsWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const int count,
                    const eAction& action,
                    const int tooltipS);

    void setCount(const int count);
private:
    eLabel* mLabel = nullptr;
};

#endif // ECOINSWIDGET_H
