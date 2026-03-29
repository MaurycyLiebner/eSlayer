#ifndef EINVENTORYBAGPACKWIDGET_H
#define EINVENTORYBAGPACKWIDGET_H

#include "../ewidget.h"

struct eEquipment;
struct eInventoryItem;
struct eItemData;

class eInventoryBagpackWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(eEquipment* const eq,
                    const int dimensions);

    bool dropItem(const SDL_Point& mpos);
protected:
    void paintEvent(ePainter& p) override;
    bool mousePressEvent(const eMouseEvent& e) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
    bool mouseLeaveEvent(const eMouseEvent& e) override;
private:
    int itemIdAt(const SDL_Point& ipos) const;
    std::vector<int> itemIdsAt(const SDL_Rect& irect) const;
    bool rectInBounds(const SDL_Rect& irect) const;
    SDL_Rect itemDropRect(const SDL_Point& ipos,
                          const eItemData& itemData) const;

    SDL_Point mousePosToItemPos(const SDL_Point& mpos);

    int mWidth = 0;
    int mHeight = 0;
    int mDimensions = 0;

    eEquipment* mEq = nullptr;
};

#endif // EINVENTORYBAGPACKWIDGET_H
