#include "eSlayerHelpers/esellers.h"

#include "eSlayerHelpers/epacket.h"

int eSeller::sSellerPageWidth = 10;
int eSeller::sSellerPageHeight = 12;

std::map<uint32_t, eSeller> eSellers::sSellers;

eInventoryItems& eSeller::addPage() {
    return fPages.emplace_back(sSellerPageWidth,
                               sSellerPageHeight);
}

eInventoryItems& eSeller::addClientPage(
    const uint32_t clientId) {
    auto& page = fClientPage[clientId];
    page = eInventoryItems(sSellerPageWidth,
                           sSellerPageHeight);
    return page;
}

bool eSeller::takeItem(const uint32_t clientId,
                       const uint32_t itemId) {
    const auto r = fClientPage[clientId].take(itemId);
    if(r.fType != eItemType::none) return true;
    for(auto& p : fPages) {
        const auto r = p.take(itemId);
        if(r.fType == eItemType::none) continue;
        return true;
    }
    return false;
}

eItem eSeller::item(const uint32_t clientId,
                    const uint32_t itemId) const {
    const auto it = fClientPage.find(clientId);
    if(it != fClientPage.end()) {
        const auto& c = it->second;
        const auto r = c.item(itemId);
        if(r.fType != eItemType::none) return r;
    }
    for(auto& p : fPages) {
        const auto r = p.item(itemId);
        if(r.fType == eItemType::none) continue;
        return r;
    }
    return eItem();
}

bool eSeller::setItemId(const uint32_t clientId,
                        const uint32_t itemId,
                        const uint32_t newItemId) {
    const auto it = fClientPage.find(clientId);
    if(it != fClientPage.end()) {
        auto& c = it->second;
        const bool r = c.setItemId(itemId, newItemId);
        if(r) return true;
    }
    for(auto& p : fPages) {
        const bool r = p.setItemId(itemId, newItemId);
        if(r) return true;
    }
    return false;
}

void eSeller::read(const uint32_t clientId,
                   ePacket& p) {
    p >> fId;
    uint8_t nPages;
    p >> nPages;
    for(int i = 0; i < nPages; i++) {
        auto& page = addPage();
        page.read(p);
    }

    auto& page = addClientPage(clientId);
    page.read(p);
}

void eSeller::write(const uint32_t clientId,
                    ePacket& p) const {
    p << fId;
    const uint8_t nPages = fPages.size();
    p << nPages;
    for(auto& page : fPages) {
        page.write(p);
    }

    const auto it = fClientPage.find(clientId);
    if(it == fClientPage.end()) {
        const eInventoryItems items{
            sSellerPageWidth, sSellerPageHeight};
        items.write(p);
    } else {
        const auto& items = it->second;
        items.write(p);
    }
}

eItem eSellers::item(const uint32_t clientId,
                     const uint32_t sellerId,
                     const uint32_t itemId) {
    const auto it = sSellers.find(sellerId);
    if(it == sSellers.end()) return eItem();
    const auto& s = it->second;
    return s.item(clientId, itemId);
}

bool eSellers::takeItem(const uint32_t clientId,
                        const uint32_t sellerId,
                        const uint32_t itemId) {
    const auto it = sSellers.find(sellerId);
    if(it == sSellers.end()) return false;
    auto& s = it->second;
    return s.takeItem(clientId, itemId);
}

bool eSellers::replaceItemId(
    const uint32_t clientId,
    const eReplaceItemId& r) {
    const auto it = sSellers.find(r.fSellerId);
    if(it == sSellers.end()) return false;
    auto& s = it->second;
    return s.setItemId(clientId, r.fOldItemId, r.fNewItemId);
}
