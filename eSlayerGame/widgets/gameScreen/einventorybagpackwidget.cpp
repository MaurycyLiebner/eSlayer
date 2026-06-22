#include "einventorybagpackwidget.h"

#include "../../textures/euitextures.h"
#include "../../textures/etextgenerator.h"
#include "../../textures/etexturecolorholder.h"
#include "../../textures/eiteminstancetexture.h"
#include "einventorywidget.h"
#include "ehoverwidget.h"
#include "egamewidget.h"

#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/eitemsdata.h>

#include <SDL3/SDL_mouse.h>

void eInventoryBagpackWidget::initialize(
    const int w, const int h,
    std::vector<eInventoryItem>& items,
    eEquipment& eq, const eStats& stats,
    const eBagpackType type) {
    mEq = &eq;
    mStats = &stats;
    mItems = &items;
    mType = type;
    const auto& boxTex = eUITextures::sEmptySlot;
    mDimensions = boxTex->width();
    mWidth = w;
    mHeight = h;
    resize(mWidth*mDimensions, mHeight*mDimensions);
}

bool eInventoryBagpackWidget::dropItem() {
    const auto mpos = mousePos();
    if(eInventoryWidget::sBlocked) return false;
    auto& dragged = mEq->fDragged;
    if(dragged.fType == eItemType::none) return false;
    switch(mType) {
    case eBagpackType::belt:
    case eBagpackType::beltExtension: {
        if(dragged.fType != eItemType::potion) return false;
    } break;
    default:
        break;
    }

    const auto ipos = mousePosToItemPos(mpos);
    const auto& itemData = eItemsData::get(dragged.fDataId);
    const auto dropRect = itemDropRect(ipos, itemData);
    const bool r = rectInBounds(dropRect);
    if(!r) return false;
    const auto ids = itemIdsAt(dropRect);
    if(ids.size() > 1) return true;
    eEquipmentAction a;
    auto& place = a.fPlace;
    switch(mType) {
    case eBagpackType::belt:
        place.fType = ePlaceType::beltPotions;
        break;
    case eBagpackType::beltExtension:
        place.fType = ePlaceType::beltHiddenPotions;
        break;
    case eBagpackType::stash:
        place.fType = ePlaceType::stash;
        break;
    case eBagpackType::inventory:
        place.fType = ePlaceType::inventory;
        break;
    }
    place.fX = dropRect.x;
    place.fY = dropRect.y;
    if(ids.size() == 1) {
        auto& invItem = (*mItems)[ids[0]];
        std::swap(dragged, invItem.fItem);
        invItem.fX = dropRect.x;
        invItem.fY = dropRect.y;
        invItem.fW = dropRect.w;
        invItem.fH = dropRect.h;

        a.fItemId1 = dragged.fItemId;
        a.fType = eEquipmentActionType::switchDrag;
    } else {
        eInventoryItem invItem;
        invItem.fItem = dragged;
        invItem.fX = dropRect.x;
        invItem.fY = dropRect.y;
        invItem.fW = dropRect.w;
        invItem.fH = dropRect.h;
        mItems->push_back(invItem);
        setHoverItem(invItem);
        dragged = eItem();

        a.fType = eEquipmentActionType::drop;
    }
    eHoverWidget::sUpdateDragItem(*mEq);
    eGameWidget::sSendEqAction(a);
    return true;
}

void eInventoryBagpackWidget::setHoverItem(
    const eInventoryItem& invItem) {
    const auto& item = invItem.fItem;
    int x = invItem.fX*mDimensions;
    int y = invItem.fY*mDimensions;
    mapToGlobal(x, y);
    const int w = invItem.fW*mDimensions;
    const int h = invItem.fH*mDimensions;
    const SDL_Rect rect{x, y, w, h};
    eHoverWidget::sSetHoverItem(item, rect);
}

void eInventoryBagpackWidget::paintEvent(ePainter& p) {
    SDL_FColor fillColor{0.f, 0.f, 0.f, 1.f};
    SDL_Rect ihoverRect = {0, 0, 0, 0};
    const auto& dragged = mEq->fDragged;
    const auto mpos = mousePos();
    const auto ipos = mousePosToItemPos(mpos);
    if(dragged.fType == eItemType::none) {
        const int itemId = itemIdAt(ipos);
        if(itemId != -1) {
            const auto& item = (*mItems)[itemId];
            ihoverRect = SDL_Rect{item.fX, item.fY,
                                  item.fW, item.fH};
            fillColor = SDL_FColor{0.f, 0.7f, 0.f, 1.f};
        }
    } else {
        const auto& itemData = eItemsData::get(dragged.fDataId);
        const auto dropRect = itemDropRect(ipos, itemData);
        const bool r = rectInBounds(dropRect);
        if(r) {
            const auto ids = itemIdsAt(dropRect);
            if(ids.size() > 1) {
                ihoverRect = dropRect;
                fillColor = SDL_FColor{0.7f, 0.f, 0.f, 1.f};
            } else if(ids.size() == 1) {
                auto& invItem = (*mItems)[ids[0]];
                ihoverRect.x = invItem.fX;
                ihoverRect.y = invItem.fY;
                ihoverRect.w = invItem.fW;
                ihoverRect.h = invItem.fH;
                fillColor = SDL_FColor{0.f, 0.7f, 0.f, 1.f};
            } else { // 0
                ihoverRect = dropRect;
                fillColor = SDL_FColor{0.f, 0.7f, 0.f, 1.f};
            }
        }
    }

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
        const auto& item = i.fItem;
        const eItemInstanceTexture tex(r, res, item);
        const auto mod = tex.request();
        const int w = i.fW*mDimensions;
        const int h = i.fH*mDimensions;
        p.drawTexture(SDL_Rect{x, y, w, h}, mod.fTex, eAlignment::center);
    }

    if(mType == eBagpackType::belt) {
        if(mBeltNumbers.size() < mWidth) {
            const auto& res = resolution();
            const int fontSize = res.smallFontSize();
            const auto font = eFonts::defaultFont(fontSize);
            const eTextGenerator gen(r, eFontColor::white, font);
            while(mBeltNumbers.size() < mWidth) {
                const int id = mBeltNumbers.size() + 1;
                const auto tex = gen.generate(std::to_string(id));
                mBeltNumbers.emplace_back(tex);
            }
        }
        for(int x = 0; x < mWidth; x++) {
            const auto& tex = mBeltNumbers[x];
            for(int y = 0; y < mHeight; y++) {
                const SDL_Rect rect{x*mDimensions, y*mDimensions,
                                    mDimensions, mDimensions};
                p.drawTexture(rect, tex,
                              eAlignment::right | eAlignment::bottom);
            }
        }
    }
}

bool eInventoryBagpackWidget::mousePressEvent(const eMouseEvent& e) {
    if(eInventoryWidget::sBlocked) return true;
    if(mEq->fDragged.fType != eItemType::none) return true;
    const auto ipos = mousePosToItemPos({e.x(), e.y()});
    const int itemId = itemIdAt(ipos);
    if(itemId == -1) return true;
    auto& inv = *mItems;
    const auto item = inv[itemId].fItem;
    const auto b = e.button();
    if(b == eMouseButton::right) {
        if(item.fType == eItemType::potion) {
            const int x = mEq->beltX(item.fItemId);
            if(x != -1) {
                mEq->takePotion(x);
            } else {
                inv.erase(inv.begin() + itemId);
            }
            const auto gw = eGameWidget::sInstance;
            gw->consumePotion(item);
        }
    } else {
        if(e.shiftPressed() && item.fType == eItemType::potion) {
            eEquipmentPlace place;
            const bool r = mEq->addToBelt(item, &place);
            if(r) {
                inv.erase(inv.begin() + itemId);
                eHoverWidget::sSetHoverItem(eItem());

                eEquipmentAction a;
                a.fType = eEquipmentActionType::dragAndDrop;
                a.fItemId1 = item.fItemId;
                a.fPlace = place;
                eGameWidget::sSendEqAction(a);
            }
        } else {
            inv.erase(inv.begin() + itemId);
            mEq->fDragged = item;
            eHoverWidget::sUpdateDragItem(*mEq);
            eHoverWidget::sSetHoverItem(eItem());

            eEquipmentAction a;
            a.fType = eEquipmentActionType::drag;
            a.fItemId1 = item.fItemId;
            eGameWidget::sSendEqAction(a);
        }
    }
    return true;
}

bool eInventoryBagpackWidget::mouseMoveEvent(const eMouseEvent& e) {
    const auto ipos = mousePosToItemPos({e.x(), e.y()});
    const int itemId = itemIdAt(ipos);
    if(itemId == -1) {
        eHoverWidget::sSetHoverItem(eItem());
    } else {
        const auto& inv = *mItems;
        setHoverItem(inv[itemId]);
    }
    return true;
}

bool eInventoryBagpackWidget::mouseLeaveEvent(const eMouseEvent& e) {
    eHoverWidget::sSetHoverItem(eItem());
    return true;
}

int eInventoryBagpackWidget::itemIdAt(const SDL_Point& ipos) const {
    int id = 0;
    for(const auto& i : *mItems) {
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
    for(const auto& i : *mItems) {
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
    if(mpos.x < 0 || mpos.y < 0) return SDL_Point{-1, -1};
    return SDL_Point{mpos.x/mDimensions, mpos.y/mDimensions};
}
