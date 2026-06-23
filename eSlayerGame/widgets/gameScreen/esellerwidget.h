#ifndef ESELLERWIDGET_H
#define ESELLERWIDGET_H

#include "ebgwidget.h"

#include "esellerbag.h"

#include <eSlayerHelpers/esellers.h>

class eSellerWidget : public eBgWidget {
public:
    eSellerWidget(eMainWindow* const w);
    ~eSellerWidget();

    void initialize(const uint32_t clientId,
                    const eSeller& s,
                    eEquipment& eq,
                    const eStats& stats);
    bool dropItem();

    static bool sReplaceItemId(const uint32_t clientId,
                               const eReplaceItemId& r);
private:
    static eSellerWidget* sInstance;

    bool replaceItemId(const uint32_t clientId,
                       const eReplaceItemId& r);

    uint32_t mClientId = 0;
    eEquipment* mEq = nullptr;
    eSeller mSeller;
    eSellerBag* mBag = nullptr;
};

#endif // ESELLERWIDGET_H
