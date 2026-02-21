#ifndef EFONTS_H
#define EFONTS_H

#include <SDL3_ttf/SDL_ttf.h>
#include <map>
#include <string>

#include "../eresolution.h"

struct eFont {
    std::string fPath;
    int fPtSize;

    bool isNull() const { return fPath.empty(); }
};

inline bool operator<(const eFont& p0, const eFont& p1) {
    if(p0.fPath == p1.fPath) return p0.fPtSize < p1.fPtSize;
    return p0.fPath < p1.fPath;
}

class eFonts {
public:
    static TTF_Font* requestTTFFont(const eFont& font);
    static TTF_Font* defaultTTFFont(const eResolution res);
    static TTF_Font* defaultTTFFont(const int fs);

    static eFont defaultFont(const eResolution res);
    static eFont defaultFont(const int fs);
private:
    static std::map<eFont, TTF_Font*> sFonts;
};

#endif // EFONTS_H
