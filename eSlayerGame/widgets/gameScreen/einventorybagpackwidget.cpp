#include "einventorybagpackwidget.h"

#include "../../textures/eitemstextures.h"
#include "einventorywidget.h"
#include "eitemdragwidget.h"
#include "egamewidget.h"

#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/eitemsdata.h>

#include <SDL3/SDL_mouse.h>

void eInventoryBagpackWidget::initialize(
    eEquipment& eq, const eStats& stats,
    const int dimensions) {
    mEq = &eq;
    mStats = &stats;
    mDimensions = dimensions;
    mWidth = mEq->fInventoryWidth;
    mHeight = mEq->fInventoryHeight;
    resize(mWidth*mDimensions, mHeight*mDimensions);
}

bool eInventoryBagpackWidget::dropItem(const SDL_Point& mpos) {
    if(eInventoryWidget::sBlocked) return false;
    auto& dragged = mEq->fDragged;
    if(dragged.fType == eItemType::none) return false;
    const auto ipos = mousePosToItemPos(mpos);
    const auto& itemData = eItemsData::get(dragged.fDataId);
    const auto dropRect = itemDropRect(ipos, itemData);
    const bool r = rectInBounds(dropRect);
    if(!r) return false;
    const auto ids = itemIdsAt(dropRect);
    if(ids.size() > 1) return false;
    if(ids.size() == 1) {
        auto& invItem = mEq->fInventory[ids[0]];
        std::swap(dragged, invItem.fItem);
        invItem.fX = dropRect.x;
        invItem.fY = dropRect.y;
        invItem.fW = dropRect.w;
        invItem.fH = dropRect.h;
    } else {
        eInventoryItem invItem;
        invItem.fItem = dragged;
        invItem.fX = dropRect.x;
        invItem.fY = dropRect.y;
        invItem.fW = dropRect.w;
        invItem.fH = dropRect.h;
        mEq->fInventory.push_back(invItem);
        eItemDragWidget::sSetHoverItem(dragged);
        dragged = eItem();
    }
    eItemDragWidget::sUpdateDragItem(*mEq);
    eGameWidget::sSendInventoryRearranged();
    return true;
}

void eInventoryBagpackWidget::paintEvent(ePainter& p) {
    SDL_Color fillColor{0, 0, 0, 255};
    SDL_Rect ihoverRect = {0, 0, 0, 0};
    const auto& dragged = mEq->fDragged;
    const auto mpos = mousePos();
    const auto ipos = mousePosToItemPos(mpos);
    if(dragged.fType == eItemType::none) {
        const int itemId = itemIdAt(ipos);
        if(itemId != -1) {
            const auto& item = mEq->fInventory[itemId];
            ihoverRect = SDL_Rect{item.fX, item.fY,
                                  item.fW, item.fH};
            fillColor = SDL_Color{0, 128, 0, 255};
        }
    } else {
        const auto& itemData = eItemsData::get(dragged.fDataId);
        const auto dropRect = itemDropRect(ipos, itemData);
        const bool r = rectInBounds(dropRect);
        if(r) {
            const auto ids = itemIdsAt(dropRect);
            if(ids.size() > 1) {
                ihoverRect = dropRect;
                fillColor = SDL_Color{128, 0, 0, 255};
            } else if(ids.size() == 1) {
                auto& invItem = mEq->fInventory[ids[0]];
                ihoverRect.x = invItem.fX;
                ihoverRect.y = invItem.fY;
                ihoverRect.w = invItem.fW;
                ihoverRect.h = invItem.fH;
                fillColor = SDL_Color{0, 128, 0, 255};
            } else { // 0
                ihoverRect = dropRect;
                fillColor = SDL_Color{0, 128, 0, 255};
            }
        }
    }

    const auto& res = resolution();
    const int lineWidth = res.lineWidth();
    for(int x = 0; x < mWidth; x++) {
        for(int y = 0; y < mHeight; y++) {
            const SDL_Rect rect{x*mDimensions,
                                y*mDimensions,
                                mDimensions,
                                mDimensions};
            p.fillRect(rect, fillColor);
            const SDL_Point pt{x, y};
            if(SDL_PointInRect(&pt, &ihoverRect)) {
                p.fillRect(rect, fillColor);
            } else {
                p.fillRect(rect, SDL_Color{0, 0, 0, 255});
            }
            p.drawRect(rect, SDL_Color{255, 255, 255, 255}, lineWidth);
        }
    }
    const auto r = renderer();
    for(const auto& i : mEq->fInventory) {
        const int x = i.fX*mDimensions;
        const int y = i.fY*mDimensions;
        const auto& item = i.fItem;
        const int dataId = item.fDataId;
        auto& itemTex = eItemsTextures::getByItemDataId(dataId);
        itemTex.request(r);
        const auto& tex = itemTex.fTex;
        const int w = i.fW*mDimensions;
        const int h = i.fH*mDimensions;
        const bool mod = !mStats->itemReqsMet(item);
        if(mod) tex->setColorMod(255, 0, 0);
        p.drawTexture(SDL_Rect{x, y, w, h}, tex, eAlignment::center);
        if(mod) tex->clearColorMod();
    }
}

bool eInventoryBagpackWidget::mousePressEvent(const eMouseEvent& e) {
    if(eInventoryWidget::sBlocked) return true;
    if(mEq->fDragged.fType != eItemType::none) return true;
    const auto ipos = mousePosToItemPos({e.x(), e.y()});
    const int itemId = itemIdAt(ipos);
    if(itemId == -1) return true;
    auto& inv = mEq->fInventory;
    const auto item = inv[itemId].fItem;
    inv.erase(inv.begin() + itemId);
    mEq->fDragged = item;
    eItemDragWidget::sUpdateDragItem(*mEq);
    eItemDragWidget::sSetHoverItem(eItem());
    eGameWidget::sSendInventoryRearranged();
    return true;
}

bool eInventoryBagpackWidget::mouseMoveEvent(const eMouseEvent& e) {
    const auto ipos = mousePosToItemPos({e.x(), e.y()});
    const int itemId = itemIdAt(ipos);
    if(itemId == -1) {
        eItemDragWidget::sSetHoverItem(eItem());
    } else {
        const auto& inv = mEq->fInventory;
        const auto& item = inv[itemId].fItem;
        eItemDragWidget::sSetHoverItem(item);
    }
    return true;
}

bool eInventoryBagpackWidget::mouseLeaveEvent(const eMouseEvent& e) {
    eItemDragWidget::sSetHoverItem(eItem());
    return true;
}

int eInventoryBagpackWidget::itemIdAt(const SDL_Point& ipos) const {
    int id = 0;
    for(const auto& i : mEq->fInventory) {
        const SDL_Rect rect{i.fX, i.fY, i.fW, i.fH};
        const bool r = SDL_PointInRect(&ipos, &rect);
        if(r) return id;
        id++;
    }
    return -1;
}

std::vector<int> eInventoryBagpackWidget::itemIdsAt(const SDL_Rect& irect) const {
    std::vector<int> result;
    int id = 0;
    for(const auto& i : mEq->fInventory) {
        const SDL_Rect rect{i.fX, i.fY, i.fW, i.fH};
        const bool r = SDL_HasRectIntersection(&irect, &rect);
        if(r) result.push_back(id);
        id++;
    }
    return result;
}

bool eInventoryBagpackWidget::rectInBounds(const SDL_Rect& irect) const {
    return irect.x >= 0 && irect.y >= 0 &&
           irect.x + irect.w <= mWidth &&
           irect.y + irect.h <= mHeight;
}

SDL_Rect eInventoryBagpackWidget::itemDropRect(
    const SDL_Point& ipos, const eItemData& itemData) const {
    return SDL_Rect{ipos.x - itemData.fWidth/2,
                    ipos.y - itemData.fHeight/2,
                    itemData.fWidth, itemData.fHeight};
}

SDL_Point eInventoryBagpackWidget::mousePosToItemPos(
    const SDL_Point& mpos) {
    return SDL_Point{mpos.x/mDimensions, mpos.y/mDimensions};
}
