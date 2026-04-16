#ifndef EINVENTORYBAGPACKWIDGET_H
#define EINVENTORYBAGPACKWIDGET_H

#include "../ewidget.h"

struct eEquipment;
struct eInventoryItem;
struct eItemData;
struct eStats;

enum class eBagpackType {
    inventory,
    stash,
    belt,
    beltExtension
};

class eInventoryBagpackWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const int w, const int h,
                    std::vector<eInventoryItem>& items,
                    eEquipment& eq, const eStats& stats,
                    const eBagpackType type);

    bool dropItem();
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

    eBagpackType mType = eBagpackType::inventory;
    eEquipment* mEq = nullptr;
    std::vector<eInventoryItem>* mItems = nullptr;
    const eStats* mStats = nullptr;

    std::vector<std::shared_ptr<eTexture>> mBeltNumbers;
};

#endif // EINVENTORYBAGPACKWIDGET_H
