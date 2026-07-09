#ifndef EINVENTORYBAGPACKWIDGET_H
#define EINVENTORYBAGPACKWIDGET_H

#include "ebagpackbase.h"

struct eEquipment;

enum class eBagpackType {
    inventory,
    stash,
    belt,
    beltExtension
};

class eInventoryBagpackWidget : public eBagpackBase {
public:
    using eBagpackBase::eBagpackBase;

    void initialize(const int w, const int h,
                    std::vector<eInventoryItem>& items,
                    eEquipment& eq,
                    const eBagpackType type,
                    const eHoverItemType htype,
                    eItem* dragged = nullptr);

    bool dropItem();
protected:
    void paintEvent(ePainter& p) override;
    bool mousePressEvent(const eMouseEvent& e) override;
private:
    eBagpackType mType = eBagpackType::inventory;
    eEquipment* mEq = nullptr;
    eItem* mDragged = nullptr;

    std::vector<std::shared_ptr<eTexture>> mBeltNumbers;
};

#endif // EINVENTORYBAGPACKWIDGET_H
