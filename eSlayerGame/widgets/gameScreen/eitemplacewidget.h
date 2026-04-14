#ifndef EITEMPLACEWIDGET_H
#define EITEMPLACEWIDGET_H

#include "../elabel.h"

struct eEquipment;
struct eItem;
struct eStats;
enum class eItemType : uint8_t;

class eItemPlaceWidget : public eLabel {
public:
    using eLabel::eLabel;

    void intialize(const std::shared_ptr<eTexture>& tex,
                   eEquipment& eq,
                   const eStats& stats,
                   eItem eEquipment::* const item,
                   const std::vector<eItemType>& allowedTypes);

    bool dropItem();
protected:
    void paintEvent(ePainter& p) override;
    bool mousePressEvent(const eMouseEvent& e) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
    bool mouseEnterEvent(const eMouseEvent& e) override;
    bool mouseLeaveEvent(const eMouseEvent& e) override;
private:
    bool draggedCompatible();

    std::vector<eItemType> mAllowedTypes;
    eEquipment* mEq = nullptr;
    const eStats* mStats = nullptr;
    eItem eEquipment::*mDst = nullptr;
    std::shared_ptr<eTexture> mTex;
};

#endif // EITEMPLACEWIDGET_H
