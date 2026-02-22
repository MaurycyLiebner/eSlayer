#include "etextgenerator.h"

#include "etexture.h"

#include <cstdio>
#include <string>

eTextGenerator::eTextGenerator(SDL_Renderer* const r,
                               const eFontColor color,
                               const eFont& font,
                               const int shift,
                               const int width) :
    mR(r), mColor(color), mFont(font),
    mShift(shift), mWidth(width) {}

std::shared_ptr<eTexture>
gGenerateTextTexture(SDL_Renderer* const r,
                     const std::string& text,
                     const SDL_Color& color,
                     TTF_Font& font,
                     const int width) {
    const auto surf = TTF_RenderText_Blended_Wrapped(
        &font, text.c_str(), text.size(), color, width);
    if(!surf) {
        printf("Unable to render text! "
               "SDL_ttf Error: %s\n", SDL_GetError());
        return nullptr;
    }
    const auto tex = std::make_shared<eTexture>();
    tex->load(r, surf);
    return tex;
}

std::shared_ptr<eTexture>
eTextGenerator::generate(const std::string& text) const {
    const auto ttf = eFonts::requestTTFFont(mFont);
    if(!ttf) return nullptr;

    SDL_Color col1;
    SDL_Color col2;
    eFontColorHelpers::colors(mColor, col1, col2);

    const auto tex1 = gGenerateTextTexture(mR, text, col1,
                                           *ttf, mWidth);
    if(!tex1) return nullptr;
    if(col2.a == 0) return tex1;
    const auto tex2 = gGenerateTextTexture(mR, text, col2,
                                           *ttf, mWidth);
    if(!tex2) return nullptr;

    const int w = tex1->width();
    const int h = tex1->height();

    const int dx = mShift;
    const int dy = mShift;

    const auto tex = std::make_shared<eTexture>();
    const bool r = tex->create(mR, w + dx, h + dy);

    if(!r) {
        printf("Unable to create texture! "
               "SDL Error: %s\n", SDL_GetError());
        return nullptr;
    }
    {
        const auto holder = tex->createTargetHolder(mR);
        const auto bm = SDL_ComposeCustomBlendMode(
            SDL_BLENDFACTOR_ONE,
            SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            SDL_BLENDOPERATION_ADD,

            SDL_BLENDFACTOR_ONE,
            SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            SDL_BLENDOPERATION_ADD);
        tex->setBlendMode(bm);

        tex2->render(mR, 0, 0);
        tex1->render(mR, dx, dy);
    }
    return tex;
}
