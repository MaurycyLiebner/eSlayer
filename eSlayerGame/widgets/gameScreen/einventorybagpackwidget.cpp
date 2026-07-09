#include "einventorybagpackwidget.h"

#include "../../textures/etextgenerator.h"
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
    eEquipment& eq,
    const eBagpackType type,
    const eHoverItemType htype,
    eItem* dragged) {
    eBagpackBase::initialize(
        w, h, items, htype);
    mEq = &eq;
    if(!dragged) {
        mDragged = &mEq->fDragged;
    } else {
        mDragged = dragged;
    }
    mType = type;
}

bool eInventoryBagpackWidget::dropItem() {
    const auto mpos = mousePos();
    if(eInventoryWidget::sBlocked) return false;
    auto& dragged = *mDragged;
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

        bool addedJewel = false;

        if(dragged.fType == eItemType::jewel) {
            auto& dst = invItem.fItem;
            if(dst.spaceForJewel()) {
                dst.addJewel(dragged);
                eEquipmentAction a;
                a.fType = eEquipmentActionType::insertJewel;
                a.fItemId1 = dragged.fItemId;
                a.fItemId2 = dst.fItemId;
                dragged = eItem();
                addedJewel = true;
                eHoverWidget::sClearHover();
            }
        }

        if(!addedJewel) {
            std::swap(dragged, invItem.fItem);
            invItem.fX = dropRect.x;
            invItem.fY = dropRect.y;
            invItem.fW = dropRect.w;
            invItem.fH = dropRect.h;

            a.fItemId1 = dragged.fItemId;
            a.fType = eEquipmentActionType::switchDrag;
        }
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
    eHoverWidget::sUpdateDragItem(dragged);
    eGameWidget::sSendEqAction(a);
    return true;
}

void eInventoryBagpackWidget::paintEvent(ePainter& p) {
    SDL_FColor fillColor{0.f, 0.f, 0.f, 1.f};
    SDL_Rect ihoverRect = {0, 0, 0, 0};
    const auto& dragged = *mDragged;
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

    eBagpackBase::paint(p, ihoverRect, fillColor);

    const auto r = renderer();
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
    if((*mDragged).fType != eItemType::none) return true;
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
                eHoverWidget::sSetHoverItem(eHoverItem());

                eEquipmentAction a;
                a.fType = eEquipmentActionType::dragAndDrop;
                a.fItemId1 = item.fItemId;
                a.fPlace = place;
                eGameWidget::sSendEqAction(a);
            }
        } else {
            inv.erase(inv.begin() + itemId);
            *mDragged = item;
            eHoverWidget::sUpdateDragItem(*mDragged);
            eHoverWidget::sSetHoverItem(eHoverItem());

            eEquipmentAction a;
            a.fType = eEquipmentActionType::drag;
            a.fItemId1 = item.fItemId;
            eGameWidget::sSendEqAction(a);
        }
    }
    return true;
}
