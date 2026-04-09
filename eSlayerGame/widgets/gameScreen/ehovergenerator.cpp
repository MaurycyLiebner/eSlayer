#include "ehovergenerator.h"

#include "../../textures/etextgenerator.h"
#include "../epainter.h"

eHoverGenerator::eHoverGenerator(const eResolution& res) {
    const int fontSize = res.smallFontSize();
    font = eFonts::textFont(fontSize);
}

void eHoverGenerator::addText(SDL_Renderer* const r,
                              const std::string& text,
                              const eFontColor color) {
    if(text.empty()) return;
    eTextGenerator gen(r, color, font);
    const auto tex = gen.generate(text);
    totalHeight += tex->height();
    maxWidth = std::max(maxWidth, tex->width());
    lines.emplace_back(tex);
}

std::shared_ptr<eTexture>
eHoverGenerator::generate(SDL_Renderer* const r) const {
    const auto result = std::make_shared<eTexture>();
    result->create(r, maxWidth, totalHeight);
    {
        const auto h = result->createTargetHolder(r);
        ePainter p(r);
        int y = 0;
        for(const auto& l : lines) {
            p.drawTexture(maxWidth/2, y, l, eAlignment::hcenter);
            y += l->height();
        }
    }
    return result;
}

void eHoverGenerator::sPaint(const int w, const int h,
                             const int mouseX, const int mouseY,
                             const eResolution& res,
                             const std::shared_ptr<eTexture>& tex,
                             ePainter& p) {
    const int screenMargin = 40*res.multiplier();
    const int fillMargin = 10*res.multiplier();
    SDL_Rect rect{mouseX - tex->width()/2,
                  mouseY - tex->height(),
                  tex->width(), tex->height()};
    if(rect.y < screenMargin) rect.y = screenMargin;
    else if(rect.y + rect.h > h - screenMargin) rect.y = h - screenMargin - rect.h;
    if(rect.x < screenMargin) rect.x = screenMargin;
    else if(rect.x + rect.w > w - screenMargin) rect.x = w - screenMargin - rect.w;
    const SDL_Rect fillRect{rect.x - fillMargin, rect.y - fillMargin,
                            rect.w + 2*fillMargin, rect.h + 2*fillMargin};
    p.fillRect(fillRect, SDL_Color{0, 0, 0, 200});
    p.drawTexture(rect, tex, eAlignment::center);

}
