#ifndef EITEMNAMES_H
#define EITEMNAMES_H

#include "../../textures/etexture.h"

#include <map>
#include <cstdint>
#include <memory>

#include <SDL3/SDL_rect.h>

struct ePoint;
struct eGroundItem;

struct eItemName {
    uint32_t fItemId;
    std::shared_ptr<eTexture> fName;
    SDL_Rect fRect;
};

class eItemNames : public std::map<uint32_t, eItemName> {
public:
    bool add(SDL_Renderer* const r,
             const eFont& font,
             const int w, const int h,
             const ePoint& pixel,
             const eGroundItem& item);
    bool at(const ePoint& pixel,
            uint32_t& itemId) const;
private:
    std::map<uint32_t, std::shared_ptr<eTexture>> mTexs;
};

#endif // EITEMNAMES_H
