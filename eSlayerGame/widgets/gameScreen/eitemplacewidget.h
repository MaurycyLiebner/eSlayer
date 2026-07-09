#ifndef EITEMPLACEWIDGET_H
#define EITEMPLACEWIDGET_H

#include "../elabel.h"

#include <eSlayerHelpers/eequipmentplace.h>
#include <eSlayerHelpers/eitem.h>

struct eEquipment;
struct eItem;
struct eStats;
enum class eItemType : uint8_t;

class eItemPlaceWidget : public eLabel {
public:
    using eLabel::eLabel;

    void intialize(const uint32_t unitId,
                   const std::shared_ptr<eTexture>& tex,
                   eEquipment& eq,
                   const eStats* const stats,
                   eItem eEquipment::* const item,
                   const ePlaceType place,
                   const eHoverItemType htype,
                   eItem* const dragged = nullptr);

    bool dropItem();
    void setHoverItem();
protected:
    void paintEvent(ePainter& p) override;
    bool mousePressEvent(const eMouseEvent& e) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
    bool mouseEnterEvent(const eMouseEvent& e) override;
    bool mouseLeaveEvent(const eMouseEvent& e) override;
private:
    bool draggedCompatible();

    uint32_t mUnitId = 0;

    eHoverItemType mHoverType = eHoverItemType::regular;

    eEquipmentPlace mPlace;
    eEquipment* mEq = nullptr;
    const eStats* mStats = nullptr;
    eItem eEquipment::*mDst = nullptr;
    std::shared_ptr<eTexture> mTex;
    eItem* mDragged = nullptr;
};

#endif // EITEMPLACEWIDGET_H
