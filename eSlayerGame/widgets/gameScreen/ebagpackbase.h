#ifndef EBAGPACKBASE_H
#define EBAGPACKBASE_H

#include "../ewidget.h"

struct eInventoryItem;
struct eStats;
struct eItemData;

class eBagpackBase : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const int w, const int h,
                    std::vector<eInventoryItem>& items,
                    const eStats& stats);

    void setHoverItem(const eInventoryItem& invItem);
protected:
    void paint(ePainter& p,
               const SDL_Rect& ihoverRect,
               const SDL_FColor& fillColor);

    bool mouseMoveEvent(const eMouseEvent& e) override;
    bool mouseLeaveEvent(const eMouseEvent& e) override;

    int itemIdAt(const SDL_Point& ipos) const;
    std::vector<int> itemIdsAt(const SDL_Rect& irect) const;
    bool rectInBounds(const SDL_Rect& irect) const;
    SDL_Rect itemDropRect(const SDL_Point& ipos,
                          const eItemData& itemData) const;

    SDL_Point mousePosToItemPos(const SDL_Point& mpos);

    int mWidth = 0;
    int mHeight = 0;
    int mDimensions = 0;

    std::vector<eInventoryItem>* mItems = nullptr;
    const eStats* mStats = nullptr;
};

#endif // EBAGPACKBASE_H
