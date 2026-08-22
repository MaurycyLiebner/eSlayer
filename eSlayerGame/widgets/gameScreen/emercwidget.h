#ifndef EMERCWIDGET_H
#define EMERCWIDGET_H

#include "einventorywidget.h"
#include "estatswidget.h"

#include <eSlayerHelpers/emercenary.h>
#include <eSlayerHelpers/estats.h>

class eMercWidget : public eBgWidget {
public:
    using eBgWidget::eBgWidget;

    void initialize(eMercenary& merc, eEquipment& eq);

    bool dropItem();
protected:
    void paintEvent(ePainter& p) override;
private:
    uint32_t mUpdateCounter = 0;
    eMercenary* mMerc = nullptr;
    eStats mStats;
    eAttributes mAttributes;
    eInventoryWidgetBase* mInv = nullptr;
    eStatsWidgetBase* mStat = nullptr;
};

#endif // EMERCWIDGET_H
