#ifndef ECOLORS_H
#define ECOLORS_H

#include <SDL3/SDL.h>

struct eColors {
    static SDL_Color sHealth;
    static SDL_Color sHealthPoisoned;
    static SDL_Color sMana;
    static SDL_Color sStamina;
    static SDL_Color sStaminaPotion;
    static SDL_Color sExperience;
    static SDL_Color sHoverBg;
};

#endif // ECOLORS_H
