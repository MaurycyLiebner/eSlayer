#ifndef EADDSOCKETWIDGET_H
#define EADDSOCKETWIDGET_H

#include "ebgwidget.h"

class eItemPlaceWidget;

struct eEquipment;

using eTryAction = std::function<bool(void)>;

class eAddSocketWidget :
    public eBgWidget {
public:
    using eBgWidget::eBgWidget;

    void initialize(eEquipment& eq,
                    const eTryAction& applyAction,
                    const eAction& cancelAction);

    bool dropItem();
private:
    eItemPlaceWidget* mPlace = nullptr;
};

#endif // EADDSOCKETWIDGET_H
