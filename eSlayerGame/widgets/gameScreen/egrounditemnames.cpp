#include "egrounditemnames.h"

#include "../../textures/etextgenerator.h"
#include "../../names/eitemnames.h"

#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/egrounditem.h>

void eGroundItemNames::initialize(
    SDL_Renderer* const r,
    const eFont& font) {
    mR = r;
    mFont = font;
}

bool eGroundItemNames::add(
    const ePoint& pixel,
    const eGroundItem& item) {
    const auto tex = requestTexture(item);
    if(!tex) return false;
    return placeBox(tex, pixel, item);
}

bool eGroundItemNames::at(const ePoint& pixel,
                          uint32_t& itemId) const {
    const SDL_Point pt{pixel.fX, pixel.fY};
    for(const auto& it : *this) {
        const auto& i = it.second;
        const bool r = SDL_PointInRect(&pt, &i.fRect);
        if(r) {
            itemId = i.fItemId;
            return true;
        }
    }
    return false;
}

std::shared_ptr<eTexture>
eGroundItemNames::requestTexture(
    const eGroundItem& item) {
    const auto it = mTexs.find(item.fItemId);
    if(it != mTexs.end()) return it->second;
    const auto name = eItemNames::name(item.fDataId);
    eFontColor color{eFontColor::normal};
    switch(item.fRarity) {
    case eItemRarity::normal:
        color = eFontColor::normal;
        break;
    case eItemRarity::magic:
        color = eFontColor::magic;
        break;
    case eItemRarity::rare:
        color = eFontColor::rare;
        break;
    case eItemRarity::set:
        color = eFontColor::set;
        break;
    case eItemRarity::unique:
        color = eFontColor::unique;
        break;
    }

    eTextGenerator gen(mR, color, mFont);
    const auto socketsText = item.fSockets > 0 ?
                                 " [" + std::to_string(item.fSockets) + "]" :
                                 "";
    const auto tex = gen.generate(name + socketsText);
    mTexs[item.fItemId] = tex;
    return tex;
}

bool eGroundItemNames::placeBox(
    const std::shared_ptr<eTexture>& tex,
    const ePoint& pixel,
    const eGroundItem& item) {
    const int tw = tex->width();
    const int th = tex->height();
    const int ww = tw + mFont.fPtSize;
    const int wh = th + mFont.fPtSize;
    const int yinc = wh + mFont.fPtSize;
    if(empty()) {
        mYShift = pixel.fY % yinc - yinc;
    }
    for(int y = mYShift + pixel.fY/yinc*yinc; y > 0; y -= yinc) {
        const SDL_Rect rect{pixel.fX - ww/2, y, ww, wh};
        bool empty = true;
        for(const auto& it : *this) {
            const auto& i = it.second;
            const bool r = SDL_HasRectIntersection(&rect, &i.fRect);
            if(r) {
                empty = false;
                break;
            }
        }
        if(empty) {
            eItemName i;
            i.fItemId = item.fItemId;
            i.fName = tex;
            i.fRect = rect;
            (*this)[item.fItemId] = i;
            return true;
        }
    }
    return false;
}
