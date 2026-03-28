#include "einventorybagpackwidget.h"

#include "../../textures/eitemstextures.h"

#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/eitemsdata.h>

#include <SDL3/SDL_mouse.h>

void eInventoryBagpackWidget::initialize(
    const eAction& dragChange,
    eEquipment* const eq,
    const int dimensions,
    const int width,
    const int height) {
    mDraggedChanged = dragChange;
    mEq = eq;
    mDimensions = dimensions;
    mWidth = width;
    mHeight = height;
    resize(mWidth*mDimensions, mHeight*mDimensions);
}

bool eInventoryBagpackWidget::dropItem(const SDL_Point& mpos) {
    const auto& dragged = mEq->fDragged;
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
        std::swap(mEq->fDragged, invItem.fItem);
        invItem.fX = dropRect.x;
        invItem.fY = dropRect.y;
        invItem.fW = dropRect.w;
        invItem.fH = dropRect.h;
    } else {
        eInventoryItem invItem;
        invItem.fItem = mEq->fDragged;
        invItem.fX = dropRect.x;
        invItem.fY = dropRect.y;
        invItem.fW = dropRect.w;
        invItem.fH = dropRect.h;
        mEq->fInventory.push_back(invItem);
        mEq->fDragged.fType = eItemType::none;
    }
    mDraggedChanged();
    return true;
}

void eInventoryBagpackWidget::paintEvent(ePainter& p) {
    SDL_Color hoverColor;
    SDL_Rect ihoverRect = {0, 0, 0, 0};
    const auto& dragged = mEq->fDragged;
    SDL_Point ipos;
    {
        float mxf;
        float myf;
        SDL_GetMouseState(&mxf, &myf);
        int mx = mxf;
        int my = myf;
        mapFromGlobal(mx, my);
        const SDL_Point mpos{mx, my};
        ipos = mousePosToItemPos(mpos);
    }
    if(dragged.fType == eItemType::none) {
        const int itemId = itemIdAt(ipos);
        if(itemId != -1) {
            const auto& item = mEq->fInventory[itemId];
            ihoverRect = SDL_Rect{item.fX, item.fY,
                                  item.fW, item.fH};
            hoverColor = SDL_Color{0, 255, 0, 128};
        }
    } else {
        const auto& itemData = eItemsData::get(dragged.fDataId);
        const auto dropRect = itemDropRect(ipos, itemData);
        const bool r = rectInBounds(dropRect);
        if(r) {
            const auto ids = itemIdsAt(dropRect);
            if(ids.size() > 1) {
                ihoverRect = dropRect;
                hoverColor = SDL_Color{255, 0, 0, 128};
            } else if(ids.size() == 1) {
                auto& invItem = mEq->fInventory[ids[0]];
                ihoverRect.x = invItem.fX;
                ihoverRect.y = invItem.fY;
                ihoverRect.w = invItem.fW;
                ihoverRect.h = invItem.fH;
                hoverColor = SDL_Color{0, 255, 0, 128};
            } else { // 0
                ihoverRect = dropRect;
                hoverColor = SDL_Color{0, 255, 0, 128};
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
            p.fillRect(rect, SDL_Color{0, 0, 0, 255});
            const SDL_Point pt{x, y};
            if(SDL_PointInRect(&pt, &ihoverRect)) {
                p.fillRect(rect, hoverColor);
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
        const auto itemName = eItemsData::name(dataId);
        auto& itemTex = eItemsTextures::get(itemName);
        itemTex.request(r);
        const auto& tex = itemTex.fTex;
        const SDL_Point pt{i.fX, i.fY};
        if(SDL_PointInRect(&pt, &ihoverRect)) {
            tex->setColorMod(hoverColor.r, hoverColor.g, hoverColor.b);
        }
        const int w = i.fW*mDimensions;
        const int h = i.fH*mDimensions;
        p.drawTexture(SDL_Rect{x, y, w, h}, tex, eAlignment::center);
        tex->clearColorMod();
    }
}

bool eInventoryBagpackWidget::mousePressEvent(const eMouseEvent& e) {
    const auto ipos = mousePosToItemPos({e.x(), e.y()});
    const int itemId = itemIdAt(ipos);
    if(itemId == -1) return true;
    auto& inv = mEq->fInventory;
    const auto item = inv[itemId].fItem;
    inv.erase(inv.begin() + itemId);
    mEq->fDragged = item;
    mDraggedChanged();
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
