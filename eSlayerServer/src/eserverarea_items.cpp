#include "eserverarea.h"

#include "eitemgenerator.h"

std::shared_ptr<eGroundItem>
eServerArea::groundItem(const uint32_t itemId) const {
    return mGroundItems.get(itemId);
}

void eServerArea::addGroundItem(
    const ePointF& pos, const eItem& item) {
    if(item.fType == eItemType::none) return;
    const auto itemId = item.fItemId;
    const auto groundItem = std::make_shared<eGroundItem>();
    static_cast<eItemBase&>(*groundItem) = item;
    const auto baseTile = mItemTiles.posArea(pos);
    bool found = false;
    for(int dist = 0; dist < 100; dist++) {
        for(int x = dist; x >= -dist; x--) {
            for(int y = dist; y >= -dist; y--) {
                if(std::abs(x) != dist && std::abs(y) != dist) continue;
                const eArea tile{baseTile.fX + x, baseTile.fY + y};

                const auto tilePos = mItemTiles.areaPos(tile);
                const bool w = walkable(tilePos);
                if(!w) continue;

                if(!mItemTiles.hasArea(tile)) continue;
                const auto& items = mItemTiles.at(tile);
                if(!items.empty()) continue;
                groundItem->fPos = tilePos;
                mItemTiles.emplace(tile, itemId);
                found = true;
                break;
            }
            if(found) break;
        }
        if(found) break;
    }
    if(!found) return;
    mGroundItems.add(itemId, groundItem);
    mItemsOnGround.add(itemId, std::make_shared<eItem>(item));
    const auto area = itemArea(itemId);
    mItemAreas.emplace(area, itemId);
}

void eServerArea::generateItems(
    const ePointF& pos, const int level,
    const float worth) {
    float remWorth = worth;
    while(remWorth >= 0.25f) {
        const float worth = eRand::randF(0.25f, remWorth);
        generateItem(pos, level, worth);
        remWorth -= worth;
    }
}

void eServerArea::generateItems(
    const ePointF& pos,
    const std::vector<eItemDrop>& itemDrops) {
    const int nSlayers = mClientData.size();
    for(const auto& drop : itemDrops) {
        const int n = drop.fOnePerSlayer ? nSlayers : 1;
        for(int i = 0; i < n; i++) {
            const bool r = eRand::randChance(drop.fChance);
            if(r) {
                const auto typeId = drop.fType;
                const auto item = eItemGenerator::generateItem(typeId, 1, 0.f);
                addGroundItem(pos, item);
            }
        }
    }
}

void eServerArea::generateItem(
    const ePointF& pos, const int level,
    const float worth) {
    const auto item = eItemGenerator::generateItem(level, worth);
    addGroundItem(pos, item);
}

void eServerArea::generatePotion(
    const ePointF& pos, const int level,
    const float worth) {
    const auto item = eItemGenerator::generatePotion(level, worth);
    addGroundItem(pos, item);
}