#ifndef ESTASHWIDGET_H
#define ESTASHWIDGET_H

#include "ebgwidget.h"

struct eEquipment;
struct eStats;
class eCoinsWidget;
class eInventoryBagpackWidget;
class eItemPlaceWidget;

class eStashWidget : public eBgWidget {
public:
    eStashWidget(eMainWindow* const window);
    ~eStashWidget();

    void initialize(
        const uint32_t clientId,
        eEquipment& eq, const eStats& stats);

    bool dropItem();

    static eStashWidget* sInstance;
protected:
    void paintEvent(ePainter& p) override;
private:
    eInventoryBagpackWidget* mStash = nullptr;
    eCoinsWidget* mCoins = nullptr;
    std::vector<eItemPlaceWidget*> mItemPlaces;
    eEquipment* mEq = nullptr;
};

#endif // ESTASHWIDGET_H
