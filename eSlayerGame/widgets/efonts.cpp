#include "efonts.h"

#include "../egamedir.h"

std::map<eFont, TTF_Font*> eFonts::sFonts;

TTF_Font* gLoadTTFFont(const eFont& font) {
    const auto ttf = TTF_OpenFont(font.fPath.c_str(), font.fPtSize);
    if(!ttf) {
        printf("Failed to load font! SDL_ttf Error: %s\n",
               SDL_GetError());
    }
    return ttf;
}

TTF_Font* eFonts::requestTTFFont(const eFont& font) {
    const auto it = sFonts.find(font);
    if(it != sFonts.end()) return it->second;
    const auto ttf = gLoadTTFFont(font);
    if(ttf) sFonts.insert({font, ttf});
    return ttf;
}

TTF_Font* eFonts::defaultTTFFont(const eResolution res) {
    const auto font = defaultFont(res);
    return requestTTFFont(font);
;}

TTF_Font* eFonts::defaultTTFFont(const int fs) {
    const auto font = defaultFont(fs);
    return requestTTFFont(font);
}

eFont eFonts::defaultFont(const eResolution res) {
    const int fs = res.largeFontSize();
    return defaultFont(fs);
}

eFont eFonts::defaultFont(const int fs) {
    return {eGameDir::path("Fonts/light.ttf"), fs};
}
