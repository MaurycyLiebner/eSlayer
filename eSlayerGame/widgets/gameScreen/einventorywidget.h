#ifndef EINVENTORYWIDGET_H
#define EINVENTORYWIDGET_H

#include "../ewidget.h"

class eItemDragWidget;
class eInventoryBagpackWidget;
struct eEquipment;

class eInventoryWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const eAction& dragChange,
                    eEquipment* const eq);

    bool dropItem(const SDL_Point& pos);
protected:
    void paintEvent(ePainter& p) override;
private:
    eInventoryBagpackWidget* mBagpack = nullptr;
    eEquipment* mEq = nullptr;
};

#endif // EINVENTORYWIDGET_H
