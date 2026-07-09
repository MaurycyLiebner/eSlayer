#ifndef EMERCWIDGET_H
#define EMERCWIDGET_H

#include "einventorywidget.h"

#include <eSlayerHelpers/emercenary.h>

class eMercWidget : public eBgWidget {
public:
    using eBgWidget::eBgWidget;

    void initialize(eMercenary& merc,
                    eEquipment& eq,
                    const eStats& stats);

    bool dropItem();
private:
    eInventoryWidgetBase* mInv = nullptr;
};

#endif // EMERCWIDGET_H
