#ifndef EFONTCOLOR_H
#define EFONTCOLOR_H

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

enum class eFontColor {
    white, gray, blue, red, green,
    // items
    normal, magic, rare, set, unique,
    whiteBlack, grayBlack, redBlack,
    // immunities
    fire, cold, lightning, poison, physical
};

namespace eFontColorHelpers {
    void colors(const eFontColor c,
                SDL_Color& col1,
                SDL_Color& col2);
};

#endif // EFONTCOLOR_H
