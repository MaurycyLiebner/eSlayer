#include "echarunitmodel.h"

#include "echarmodel.h"

#include "../widgets/epainter.h"

eCharUnitModel::eCharUnitModel() {}

void eCharUnitModel::setCharModel(const eCharModel& model) {
    mModel = model;
}

void eCharUnitModel::draw(ePainter& p, const int frame) {
    const int fMax = mModel.nFrames(0);
    const int gMax = mModel.nGroups();

    SDL_Rect texRect{0, 0, 0, 0};

    for(int g = 0; g < gMax; g++) {
        const int ppMax = mModel.nParts(g);
        for(int pp = 0; pp < ppMax; pp++) {
            const auto tex = mModel.get(mAnim, g, pp, mDir, frame % fMax);

            const int newX = std::min(texRect.x, tex->offsetX());
            texRect.w += texRect.x - newX;
            texRect.x = newX;
            texRect.w = std::max(texRect.w, -texRect.x + (tex->offsetX() + tex->width()));

            const int newY = std::min(texRect.y, tex->offsetY());
            texRect.h += texRect.y - newY;
            texRect.y = newY;
            texRect.h = std::max(texRect.h, -texRect.y + (tex->offsetY() + tex->height()));
        }
    }

    const auto shadow = std::make_shared<eTexture>();
    shadow->create(p.renderer(), texRect.w, texRect.h);

    ePainter sp(p.renderer());
    sp.translate(-texRect.x, -texRect.y);
    shadow->setAsRenderTarget(sp.renderer());
    for(int g = 0; g < gMax; g++) {
        const int ppMax = mModel.nParts(g);
        for(int pp = 0; pp < ppMax; pp++) {
            const auto tex = mModel.get(mAnim, g, pp, mDir, frame % fMax);
            sp.drawTexture(tex->offsetX(), tex->offsetY(), tex);
        }
    }
    SDL_SetRenderTarget(p.renderer(), nullptr);

    {
        float skew = 0.5f;
        float scaleY = 0.5f;

        const float x = p.x() - shadow->width();
        const float y = p.y() - shadow->height()*scaleY;

        float w = shadow->width();
        float h = shadow->height() * scaleY;

        // Bottom horizontal shift caused by skew
        float skewOffset = h * skew;

        SDL_Vertex verts[4];

        // Top-left
        verts[0].position = { x, y };
        verts[0].tex_coord = { 0.0f, 0.0f };
        verts[0].color = { 0., 0., 0., 0.5 };

        // Top-right
        verts[1].position = { x + w, y };
        verts[1].tex_coord = { 1.0f, 0.0f };
        verts[1].color = { 0., 0., 0., 0.5 };

        // Bottom-right (skewed)
        verts[2].position = { x + w + skewOffset, y + h };
        verts[2].tex_coord = { 1.0f, 1.0f };
        verts[2].color = { 0., 0., 0., 0.5 };

        // Bottom-left (skewed)
        verts[3].position = { x + skewOffset, y + h };
        verts[3].tex_coord = { 0.0f, 1.0f };
        verts[3].color = { 0., 0., 0., 0.5 };

        int indices[6] = { 0, 1, 2, 0, 2, 3 };

        SDL_RenderGeometry(p.renderer(), shadow->tex(), verts, 4, indices, 6);
    }

    for(int g = 0; g < gMax; g++) {
        const int ppMax = mModel.nParts(g);
        for(int pp = 0; pp < ppMax; pp++) {
            const auto tex = mModel.get(mAnim, g, pp, mDir, frame % fMax);
            p.drawTexture(tex->offsetX(), tex->offsetY(), tex);
        }
    }
}
