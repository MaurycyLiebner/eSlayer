#ifndef ESELLERBAG_H
#define ESELLERBAG_H

#include "ebagpackbase.h"

struct eEquipment;

class eSellerBag : public eBagpackBase {
public:
    using eBagpackBase::eBagpackBase;

    void initialize(const uint32_t sellerId,
                    const int w, const int h,
                    std::vector<eInventoryItem>& items,
                    const eStats& stats, eEquipment& eq);
protected:
    bool mousePressEvent(const eMouseEvent& e) override;
    void paintEvent(ePainter& p) override;
private:
    uint32_t mSellerId = 0;
    eEquipment* mEq = nullptr;
    const eStats* mStats = nullptr;
};

#endif // ESELLERBAG_H
