#include "epainter.h"

#include "../textures/etextgenerator.h"

ePainter::ePainter(SDL_Renderer* const renderer) :
    mRenderer(renderer) {

}

void ePainter::save() {
    ePainterSave save;
    save.fX = mX;
    save.fY = mY;
    save.fFont = mFont;
    mSaves.push(save);
}

void ePainter::restore() {
    if(mSaves.empty()) return;
    const auto save = mSaves.top();
    mSaves.pop();
    mX = save.fX;
    mY = save.fY;
    mFont = save.fFont;
}

void ePainter::translate(const int x, const int y) {
    mX += x;
    mY += y;
}

void ePainter::setFont(const eFont& font) {
    mFont = font;
}

void ePainter::drawCoordinates(int& x, int& y,
                               const int texW, const int texH,
                               const eAlignment align) {
    if(align & eAlignment::left) {
        x -= texW;
    } else if(align & eAlignment::hcenter) {
        x -= texW/2;
    }

    if(align & eAlignment::top) {
        y -= texH;
    } else if(align & eAlignment::vcenter) {
        y -= texH/2;
    }
}

void ePainter::drawTexture(int x, int y,
                           const std::shared_ptr<eTexture>& tex,
                           const eAlignment align) const {
    const int texW = tex->width();
    const int texH = tex->height();
    drawCoordinates(x, y, texW, texH, align);
    drawTexture(x, y, tex);
}

void ePainter::drawCoordinates(const SDL_Rect& rect,
                               const int texW, const int texH,
                               const eAlignment align,
                               int& x, int& y) {
    if(align & eAlignment::right) {
        x = rect.x + rect.w - texW;
    } else if(align & eAlignment::hcenter) {
        x = rect.x + (rect.w - texW)/2;
    } else {
        x = rect.x;
    }

    if(align & eAlignment::bottom) {
        y = rect.y + rect.h - texH;
    } else if(align & eAlignment::vcenter) {
        y = rect.y + (rect.h - texH)/2;
    } else {
        y = rect.y;
    }
}

void ePainter::drawTexture(const SDL_Rect& rect,
                           const std::shared_ptr<eTexture>& tex,
                           const eAlignment align) const {
    int x;
    int y;
    const int texW = tex->width();
    const int texH = tex->height();
    drawCoordinates(rect, texW, texH, align, x, y);
    drawTexture(x, y, tex);
}

void ePainter::drawTexture(const int x, const int y,
                           const std::shared_ptr<eTexture>& tex) const {
    tex->render(mRenderer, mX + x, mY + y);
}

void ePainter::fillRect(const SDL_Rect& rect,
                        const SDL_Color& color) const {
    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a);
    const SDL_FRect dRect{float(rect.x + mX),
                          float(rect.y + mY),
                          float(rect.w),
                          float(rect.h)};
    SDL_RenderFillRect(mRenderer, &dRect);
}

void ePainter::drawRect(const SDL_Rect& rect,
                        const SDL_Color& color,
                        const int width) {
    const SDL_Rect r1{rect.x,
                      rect.y,
                      rect.w,
                      width};
    const SDL_Rect r2{rect.x,
                      rect.y + rect.h - width,
                      rect.w,
                      width};
    const SDL_Rect r3{rect.x,
                      rect.y + width,
                      width,
                      rect.h - 2*width};
    const SDL_Rect r4{rect.x + rect.w - width,
                      rect.y + + width,
                      width,
                      rect.h - 2*width};
    fillRect(r1, color);
    fillRect(r2, color);
    fillRect(r3, color);
    fillRect(r4, color);
}

void ePainter::drawText(const int x, const int y,
                        const std::string& text,
                        const eFontColor color,
                        const eAlignment align) const {
    const eTextGenerator textGenerator(mRenderer, color, mFont, 0);
    const auto tex = textGenerator.generate(text);

    drawTexture(x, y, tex, align);
}

void ePainter::drawText(const SDL_Rect& rect,
                        const std::string& text,
                        const eFontColor color,
                        const eAlignment align) const {
    const eTextGenerator textGenerator(mRenderer, color, mFont, 0);
    const auto tex = textGenerator.generate(text);

    drawTexture(rect, tex, align);
}

void ePainter::drawPolygon(std::vector<SDL_FPoint> pts,
                           const SDL_Color& color) const {
    for(auto& pt : pts) {
        pt.x += mX;
        pt.y += mY;
    }
    SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderLines(mRenderer, pts.data(), pts.size());
}

void ePainter::setClipRect(const SDL_Rect* const rect) {
    if(rect) {
        const SDL_Rect r{rect->x + mX,
                         rect->y + mY,
                         rect->w, rect->h};
        SDL_SetRenderClipRect(mRenderer, &r);
    } else {
        SDL_SetRenderClipRect(mRenderer, nullptr);
    }
}
