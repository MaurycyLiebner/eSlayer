#include "egamepainter.h"

std::shared_ptr<eTexture>
eGamePainter::initialize(const int w, const int h) {
    const auto r = renderer();

    mBaseTex = std::make_shared<eTexture>();
    mBaseTex->create(r, w, h, {0, 0, 0, 255});

    mLightingTex = std::make_shared<eLightingTexture>();
    mLightingTex->initialize(r, w, h, SDL_Color{mLight, mLight, mLight, 255});

    mDisplayTex = std::make_shared<eTexture>();
    mDisplayTex->create(r, w, h, {0, 0, 0, 255});

    mItemNames = std::make_shared<eTexture>();
    mItemNames->create(r, w, h, {0, 0, 0, 0});

    return mDisplayTex;
}

eRenderTargetHolder eGamePainter::switchToLighting() {
    const auto r = renderer();
    return mLightingTex->createTargetHolder(r);
}

eRenderTargetHolder eGamePainter::switchToBase() {
    const auto r = renderer();
    return mBaseTex->createTargetHolder(r);
}

eRenderTargetHolder eGamePainter::switchToItemNames() {
    const auto r = renderer();
    mRenderItemNames = true;
    return mItemNames->createTargetHolder(r);
}

void eGamePainter::drawShadow(
    const int drawX, const int drawY,
    const eTexture& tex) {
    const float skew = 0.5f;
    const float scaleY = 0.5f;

    const float w = tex.width();
    const float h = tex.height() * scaleY;

    const float skewOffset = h * skew;

    const float x = ePainter::x() + drawX - skewOffset;
    const float y = ePainter::y() + drawY - h;

    SDL_Vertex verts[4];

    const auto& atlas = tex.atlas();
    const auto r = renderer();

    verts[0].position = { x, y };
    verts[1].position = { x + w, y };
    verts[2].position = { x + w + skewOffset, y + h };
    verts[3].position = { x + skewOffset, y + h };

    for(auto& v : verts) {
        v.color = { 0.f, 0.f, 0.f, 0.5f };
    }

    float u0 = 0.f;
    float v0 = 0.f;
    float u1 = 1.f;
    float v1 = 1.f;
    SDL_Texture* sdlTex = nullptr;

    if(atlas) {
        const float invW = 1.f / atlas->width();
        const float invH = 1.f / atlas->height();

        const float tx = tex.x();
        const float ty = tex.y();
        const float tw = tex.width();
        const float th = tex.height();

        u0 = tx * invW;
        v0 = ty * invH;
        u1 = (tx + tw) * invW;
        v1 = (ty + th) * invH;

        sdlTex = atlas->tex();
    } else {
        sdlTex = tex.tex();
    }

    verts[0].tex_coord = { u0, v0 };
    verts[1].tex_coord = { u1, v0 };
    verts[2].tex_coord = { u1, v1 };
    verts[3].tex_coord = { u0, v1 };

    static constexpr int indices[6] = { 0, 1, 2, 0, 2, 3 };

    SDL_RenderGeometry(r, sdlTex, verts, 4, indices, 6);
}

void eGamePainter::setLightness(const Uint8 light) {
    mLight = light;
    mLightingTex->setClearColor(SDL_Color{light, light, light, 255});
}

void eGamePainter::clear() {
    const auto r = renderer();
    mLightingTex->clear(r);
    mBaseTex->fill(r, SDL_Color{0, 0, 0, 255});
    if(mRenderItemNames) {
        mRenderItemNames = false;
        mItemNames->fill(r, SDL_Color{0, 0, 0, 0});
    }
}

void eGamePainter::renderLight(SDL_Renderer* const r,
                               const float x, const float y,
                               const float radius,
                               const SDL_Color& color) {
    if(mLight == 255) return;
    mLightingTex->renderLight(r, x, y, radius, color);
}

void eGamePainter::finish() {
    const auto r = renderer();
    const auto holder = mDisplayTex->createTargetHolder(r);
    mBaseTex->setBlendMode(SDL_BLENDMODE_BLEND);
    mBaseTex->render(r, 0, 0);
    if(mLight != 255) {
        mLightingTex->render(r, 0, 0);
    }
    const Uint8 a = 255 - mContrast;
    if(a != 255) {
        mBaseTex->fill(r, SDL_Color{255, 255, 255, a});
        mBaseTex->setBlendMode(SDL_BLENDMODE_MUL);
        mBaseTex->render(r, 0, 0);
    }
    if(mRenderItemNames) {
        mItemNames->render(r, 0, 0);
    }
}
