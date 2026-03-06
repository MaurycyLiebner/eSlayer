#include "echarunitmodel.h"

#include "echarmodel.h"

#include "../widgets/epainter.h"

#include <eSlayerHelpers/eexceptions.h>

#include <cmath>

eCharUnitModel::eCharUnitModel() {}

void eCharUnitModel::setCharModel(const eCharModel& model) {
    mModel = model;
}

void eCharUnitModel::incFrame(const double by) {
    mFrame += by;
    if(mAnim != 0 && mAnim != 1) {
        const int fMax = mModel.nFrames(mAnim);
        if(int(std::round(mFrame)) >= fMax) {
            setAnimation(0);
        }
    }
}

void eCharUnitModel::draw(ePainter& p) const {
    const int fMax = mModel.nFrames(mAnim);
    const int gMax = mModel.nGroups();

    const int frame = int(std::round(mFrame)) % fMax;

    SDL_Rect texRect{0, 0, 0, 0};

    for(int g = 0; g < gMax; g++) {
        const int ppMax = mModel.nParts(g);
        for(int pp = 0; pp < ppMax; pp++) {
            const auto tex = mModel.get(mAnim, g, pp, mDir, frame);

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

    const auto r = p.renderer();
    const auto shadow = std::make_shared<eTexture>();

    {
        shadow->create(r, texRect.w, texRect.h);
        ePainter sp(r);
        sp.translate(-texRect.x, -texRect.y);
        const auto holder = shadow->createTargetHolder(r);
        for(int g = 0; g < gMax; g++) {
            const int ppMax = mModel.nParts(g);
            for(int pp = 0; pp < ppMax; pp++) {
                const auto tex = mModel.get(mAnim, g, pp, mDir, frame);
                sp.drawTexture(0, 0, tex);
            }
        }
    }

    p.save();
    const auto& offset = mModel.animOffset(mAnim);
    p.translate(offset.fX, offset.fY);
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
            const auto& tex = mModel.get(mAnim, g, pp, mDir, frame);
            p.drawTexture(0, 0, tex);
        }
    }
    p.restore();
    p.fillRect(SDL_Rect{-2, -2, 4, 4}, SDL_Color{255, 0, 0, 255});
}

void eCharUnitModel::setAngle(const double a) {
    const int dirs = mModel.nDirs();
    const double ainc = 360./dirs;
    const int dir = std::round(a/ainc) - 2*dirs/16;
    setDirection((dirs + dir) % dirs);
}

void eCharUnitModel::setAnimation(const int a, const int id) {
    if(id <= mAnimId) return;
    mAnimId = id;
    setAnimation(a);
}

void eCharUnitModel::setAnimation(const int a) {
    if(a >= mModel.nAnims()) {
        eExceptions::logError("Animation id " +
                              std::to_string(a) +
                              " out of range!");
        return;
    }
    if(mAnim != a) {
        mAnim = a;
        mFrame = 0.;
    }
}

void eCharUnitModel::setDirection(const int d) {
    if(d >= mModel.nDirs()) {
        eExceptions::logError("Direction id " +
                              std::to_string(d) +
                              " out of range!");
        return;
    }
    mDir = d;
}
