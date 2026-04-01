#include "efonts.h"

#include "../efileloader.h"

std::map<eFont, TTF_Font*> eFonts::sFonts;

TTF_Font* eFonts::requestTTFFont(const eFont& font) {
    const auto it = sFonts.find(font);
    if(it != sFonts.end()) return it->second;
    const auto ttf = eFileLoader::loadTTFFont(
        font.fPtSize, "Fonts", font.fPath);
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
    return {"light.ttf", fs};
}

eFont eFonts::textFont(const eResolution res) {
    const int fs = res.largeFontSize();
    return textFont(fs);
}

eFont eFonts::textFont(const int fs) {
    return {"FreeSans.ttf", fs};
}
