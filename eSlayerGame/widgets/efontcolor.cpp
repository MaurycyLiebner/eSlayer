#include "efontcolor.h"

void eFontColorHelpers::colors(const eFontColor c,
                               SDL_Color& col1,
                               SDL_Color& col2) {
    switch(c) {
    case eFontColor::white:
        col1 = SDL_Color{255, 255, 255, 255};
        col2 = SDL_Color{0, 0, 0, 0};
        break;
    case eFontColor::gray:
        col1 = SDL_Color{155, 155, 155, 255};
        col2 = SDL_Color{0, 0, 0, 0};
        break;
    case eFontColor::blue:
        col1 = SDL_Color{97, 97, 232, 255};
        col2 = SDL_Color{0, 0, 0, 0};
        break;
    case eFontColor::red:
        col1 = SDL_Color{232, 97, 97, 255};
        col2 = SDL_Color{0, 0, 0, 0};
        break;
    case eFontColor::green:
        col1 = SDL_Color{97, 232, 97, 255};
        col2 = SDL_Color{0, 0, 0, 0};
        break;

    case eFontColor::normal:
        col1 = SDL_Color{255, 255, 255, 255};
        col2 = SDL_Color{0, 0, 0, 0};
        break;
    case eFontColor::magic:
        col1 = SDL_Color{97, 97, 232, 255};
        col2 = SDL_Color{0, 0, 0, 0};
        break;
    case eFontColor::rare:
        col1 = SDL_Color{200, 215, 0, 255};
        col2 = SDL_Color{0, 0, 0, 0};
        break;
    case eFontColor::set:
        col1 = SDL_Color{75, 220, 75, 255};
        col2 = SDL_Color{0, 0, 0, 0};
        break;
    case eFontColor::unique:
        col1 = SDL_Color{188, 160, 75, 255};
        col2 = SDL_Color{0, 0, 0, 0};
        break;

    case eFontColor::whiteBlack:
        col1 = SDL_Color{255, 255, 255, 255};
        col2 = SDL_Color{0, 0, 0, 255};
        break;
    case eFontColor::grayBlack:
        col1 = SDL_Color{155, 155, 155, 255};
        col2 = SDL_Color{0, 0, 0, 255};
        break;
    case eFontColor::redBlack:
        col1 = SDL_Color{207, 27, 2, 255};
        col2 = SDL_Color{0, 0, 0, 255};
        break;
    }
}
