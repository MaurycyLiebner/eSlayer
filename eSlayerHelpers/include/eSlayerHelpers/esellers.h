#ifndef ESELLERS_H
#define ESELLERS_H

#include "eslayerhelpersexport.h"

#include "eequipment.h"

#include <cstdint>

enum class eSellerType {
    healer
};

struct ESLAYERHELPERS_API eSeller {
    uint32_t fId;

    uint8_t fMapId;
    eSellerType fType;

    const int fSellerPageWidth = 10;
    const int fSellerPageHeight = 16;
    std::vector<eInventoryItems> fPages;

    std::map<uint32_t, eInventoryItems> fClientPage;

    eInventoryItems& addPage();
    eInventoryItems& addClientPage(const uint32_t clientId);
    bool takeItem(const uint32_t clientId,
                  const uint32_t itemId);
    eItem item(const uint32_t clientId,
               const uint32_t itemId) const;
    bool setItemId(const uint32_t clientId,
                   const uint32_t itemId,
                   const uint32_t newItemId);

    void read(const uint32_t clientId, ePacket& p);
    void write(const uint32_t clientId, ePacket& p) const;
};

class ESLAYERHELPERS_API eSellers {
public:
    static std::map<uint32_t, eSeller> sSellers;
    static eItem item(const uint32_t clientId,
                      const uint32_t sellerId,
                      const uint32_t itemId);
    static bool takeItem(const uint32_t clientId,
                         const uint32_t sellerId,
                         const uint32_t itemId);
    static bool replaceItemId(
        const uint32_t clientId,
        const uint32_t sellerId,
        const uint32_t itemId,
        const uint32_t newItemId);
};

#endif // ESELLERS_H
