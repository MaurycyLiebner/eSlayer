#ifndef EITEMPLACEWIDGET_H
#define EITEMPLACEWIDGET_H

#include "../ewidget.h"

struct eEquipment;
struct eItem;
enum class eItemType : uint8_t;

class eItemPlaceWidget : public eWidget {
public:
    using eWidget::eWidget;

    void intialize(const int width, const int height,
                   const int dimensions,
                   eEquipment* const eq,
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

    int mWidth = 0;
    int mHeight = 0;
    int mDimensions = 0;

    std::vector<eItemType> mAllowedTypes;
    eEquipment* mEq = nullptr;
    eItem eEquipment::*mItem = nullptr;
};

#endif // EITEMPLACEWIDGET_H
