#ifndef ETEXTGENERATOR_H
#define ETEXTGENERATOR_H

#include "../widgets/efonts.h"
#include "../widgets/efontcolor.h"

#include <SDL3_ttf/SDL_ttf.h>

#include <memory>

class eTexture;

class eTextGenerator {
  public:
    eTextGenerator(SDL_Renderer* const r,
                   const eFontColor color,
                   const eFont& font,
                   const int shift = 1,
                   const int width = 0);

    std::shared_ptr<eTexture>
    generate(const std::string& text) const;
  private:
    SDL_Renderer* const mR;
    const eFontColor mColor;
    const eFont mFont;
    const int mShift;
    const int mWidth;
};

#endif // ETEXTGENERATOR_H
