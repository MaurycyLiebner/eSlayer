#include "euitextures.h"

bool eUITextures::sLoaded = false;

void eUITextures::sLoad(SDL_Renderer* const r) {
    if(sLoaded) return;
    sLoaded = true;
}
