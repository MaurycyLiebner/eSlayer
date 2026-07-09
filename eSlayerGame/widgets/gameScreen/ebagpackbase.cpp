#include "ebagpackbase.h"

#include "../../textures/euitextures.h"
#include "ehoverwidget.h"

#include <eSlayerHelpers/eequipment.h>

void eBagpackBase::initialize(
    const uint32_t unitId,
    const int w, const int h,
    std::vector<eInventoryItem>& items,
    const eHoverItemType htype) {
    mUnitId = unitId;
    mItems = &items;
    mHoverType = htype;

    const auto& boxTex = eUITextures::sEmptySlot;
    mDimensions = boxTex->width();
    mWidth = w;
    mHeight = h;
    resize(mWidth*mDimensions, mHeight*mDimensions);
}

void eBagpackBase::setHoverItem(
    const eInventoryItem& invItem) {
    const auto& item = invItem.fItem;
    int x = invItem.fX*mDimensions;
    int y = invItem.fY*mDimensions;
    mapToGlobal(x, y);
    const int w = invItem.fW*mDimensions;
    const int h = invItem.fH*mDimensions;
    const SDL_Rect rect{x, y, w, h};
    eHoverItem hitem;
    hitem.fItem = item;
    hitem.fType = mHoverType;
    eHoverWidget::sSetHoverItem(hitem, rect);
}

void eBagpackBase::paint(
    ePainter& p,
    const SDL_Rect& ihoverRect,
    const SDL_FColor& fillColor) {
    const auto& boxTex = eUITextures::sEmptySlot;
    for(int x = 0; x < mWidth; x++) {
        for(int y = 0; y < mHeight; y++) {
            const SDL_Rect rect{x*mDimensions,
                                y*mDimensions,
                                mDimensions,
                                mDimensions};
            eTextureColorSetting color;
            const SDL_Point pt{x, y};
            if(SDL_PointInRect(&pt, &ihoverRect)) {
                color.set(fillColor);
            } else {
                for(const auto& i : *mItems) {
                    const SDL_Rect iRect{i.fX, i.fY, i.fW, i.fH};
                    if(SDL_PointInRect(&pt, &iRect)) {
                        color.set(0.4f, 0.4f, 0.4f);
                        break;
                    }
                }
            }
            const eTextureColorHolder mod(color, boxTex);
            p.drawTexture(rect.x, rect.y, boxTex);
        }
    }
    const auto r = renderer();
    const auto& res = resolution();
    for(const auto& i : *mItems) {
        const int x = i.fX*mDimensions;
        const int y = i.fY*mDimensions;
        const SDL_Point pt{i.fX, i.fY};
        const bool hovered = SDL_PointInRect(&pt, &ihoverRect);
        const auto& item = i.fItem;
        const eItemInstanceTexture tex(r, res, item);
        const auto mod = tex.request();
        const int w = i.fW*mDimensions;
        const int h = i.fH*mDimensions;
        const SDL_Rect rect{x, y, w, h};
        p.drawTexture(rect, mod.fTex, eAlignment::center);
        if(hovered) {
            const uint8_t nj = tex.nJewels();
            const uint8_t ns = tex.nSockets();
            for(uint8_t i = 0; i < nj; i++) {
                const auto mod = tex.requestJewel(i);
                const auto pos = tex.jewelPosition(i, ns);
                p.drawTexture(rect.x + rect.w*pos.fX,
                              rect.y + rect.h*pos.fY,
                              mod.fTex, eAlignment::center);
            }
            for(uint8_t i = nj; i < ns; i++) {
                const auto& tex = eUITextures::sSocket;
                const auto pos = eItemInstanceTexture::jewelPosition(i, ns);
                p.drawTexture(rect.x + rect.w*pos.fX,
                              rect.y + rect.h*pos.fY,
                              tex, eAlignment::center);
            }
        }
    }
}

bool eBagpackBase::mouseMoveEvent(const eMouseEvent& e) {
    const auto ipos = mousePosToItemPos({e.x(), e.y()});
    const int itemId = itemIdAt(ipos);
    if(itemId == -1) {
        eHoverWidget::sSetHoverItem(eHoverItem());
    } else {
        const auto& inv = *mItems;
        setHoverItem(inv[itemId]);
    }
    return true;
}

bool eBagpackBase::mouseLeaveEvent(const eMouseEvent& e) {
    eHoverWidget::sSetHoverItem(eHoverItem());
    return true;
}

int eBagpackBase::itemIdAt(const SDL_Point& ipos) const {
    int id = 0;
    for(const auto& i : *mItems) {
        const SDL_Rect rect{i.fX, i.fY, i.fW, i.fH};
        const bool r = SDL_PointInRect(&ipos, &rect);
        if(r) return id;
        id++;
    }
    return -1;
}

std::vector<int> eBagpackBase::itemIdsAt(const SDL_Rect& irect) const {
    std::vector<int> result;
    int id = 0;
    for(const auto& i : *mItems) {
        const SDL_Rect rect{i.fX, i.fY, i.fW, i.fH};
        const bool r = SDL_HasRectIntersection(&irect, &rect);
        if(r) result.push_back(id);
        id++;
    }
    return result;
}

bool eBagpackBase::rectInBounds(const SDL_Rect& irect) const {
    return irect.x >= 0 && irect.y >= 0 &&
           irect.x + irect.w <= mWidth &&
           irect.y + irect.h <= mHeight;
}

SDL_Rect eBagpackBase::itemDropRect(
    const SDL_Point& ipos, const eItemData& itemData) const {
    return SDL_Rect{ipos.x - itemData.fWidth/2,
                    ipos.y - itemData.fHeight/2,
                    itemData.fWidth, itemData.fHeight};
}

SDL_Point eBagpackBase::mousePosToItemPos(
    const SDL_Point& mpos) {
    if(mpos.x < 0 || mpos.y < 0) return SDL_Point{-1, -1};
    return SDL_Point{mpos.x/mDimensions, mpos.y/mDimensions};
}