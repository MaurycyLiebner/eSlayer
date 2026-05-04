#ifndef EGROUNDITEMNAMES_H
#define EGROUNDITEMNAMES_H

#include "../../textures/etexture.h"
#include "../../widgets/efonts.h"

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

class eGroundItemNames : public std::map<uint32_t, eItemName> {
public:
    void initialize(SDL_Renderer* const r,
                    const eFont& font);
    bool add(const ePoint& pixel,
             const eGroundItem& item);
    bool at(const ePoint& pixel,
            uint32_t& itemId) const;
private:
    SDL_Renderer* mR = nullptr;
    eFont mFont;

    int mYShift = 0;
    std::map<uint32_t, std::shared_ptr<eTexture>> mTexs;
};

#endif // EGROUNDITEMNAMES_H
