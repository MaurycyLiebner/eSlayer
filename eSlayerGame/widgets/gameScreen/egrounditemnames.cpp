#include "egrounditemnames.h"

#include "../../textures/etextgenerator.h"
#include "../../names/eitemnames.h"

#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/egrounditem.h>

bool eGroundItemNames::add(
    SDL_Renderer* const r,
    const eFont& font,
    const int w, const int h,
    const ePoint& pixel,
    const eGroundItem& item) {
    std::shared_ptr<eTexture> tex;
    const auto it = mTexs.find(item.fItemId);
    if(it != mTexs.end()) {
        tex = it->second;
    } else {
        const auto name = eItemNames::name(item.fDataId);
        eTextGenerator gen(r, eFontColor::white, font);
        tex = gen.generate(name);
        mTexs[item.fItemId] = tex;
    }
    const int tw = tex->width();
    const int th = tex->height();
    const int ww = tw + font.fPtSize;
    const int wh = th + font.fPtSize;
    const int yinc = wh + font.fPtSize;
    for(int y = pixel.fY - yinc; y > 0; y -= yinc) {
        const SDL_Rect rect{pixel.fX - tw/2, y,
                            ww, wh};
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
