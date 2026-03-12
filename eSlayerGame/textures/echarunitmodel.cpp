#include "echarunitmodel.h"

#include "../widgets/epainter.h"
#include "echarmodel.h"
#include "../widgets/gameScreen/egamepainter.h"
#include "echartextures.h"

#include <eSlayerHelpers/eexceptions.h>

#include <cmath>
#include <filesystem>

eCharUnitModel::eCharUnitModel() {}

void eCharUnitModel::setCharModel(const std::shared_ptr<eCharModel>& model) {
    mModel = model;
}

SDL_Rect eCharUnitModel::boundingRect() const {
    const int fMax = mModel->nFrames(mAnim);
    const int gMax = mModel->nGroups();

    const int frame = int(std::round(mFrame)) % fMax;

    SDL_Rect texRect{0, 0, 0, 0};

    for(int g = 0; g < gMax; g++) {
        const int ppMax = mModel->nParts(g);
        for(int pp = 0; pp < ppMax; pp++) {
            const auto& tex = mModel->get(mAnim, g, pp, mDir, frame);

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

    return texRect;
}

SDL_Rect eCharUnitModel::offsetBoundingRect() const {
    SDL_Rect result = boundingRect();
    const auto& offset = mModel->animOffset(mAnim);
    result.x += offset.fX;
    result.y += offset.fY;
    return result;
}

void eCharUnitModel::incFrame(const double by) {
    mFrame += by;
    if(!mClamp.empty()) {
        const int fMax = mModel->nFrames(mAnim);
        if(int(std::round(mFrame)) >= fMax) {
            const auto& data = mModel->data();
            const int animId = data.animId(mClamp);
            setAnimation(animId);
        }
    }
}

void eCharUnitModel::draw(eGamePainter& p, const bool highlight) const {
    drawBase(p);
    if(!highlight) return;

    const int fMax = mModel->nFrames(mAnim);
    const int gMax = mModel->nGroups();

    const int frame = int(std::round(mFrame)) % fMax;
    p.save();
    const auto& offset = mModel->animOffset(mAnim);
    p.translate(offset.fX, offset.fY);

    for(int g = 0; g < gMax; g++) {
        const int ppMax = mModel->nParts(g);
        for(int pp = 0; pp < ppMax; pp++) {
            const auto& tex = mModel->get(mAnim, g, pp, mDir, frame);
            tex->setBlendMode(SDL_BLENDMODE_ADD);
            tex->setAlpha(125);
            p.drawTexture(0, 0, tex);
            tex->setBlendMode(SDL_BLENDMODE_BLEND);
            tex->clearAlphaMod();
        }
    }

    p.restore();
}

void eCharUnitModel::drawBase(ePainter& p) const {
    const int fMax = mModel->nFrames(mAnim);
    const int gMax = mModel->nGroups();

    const int frame = int(std::round(mFrame)) % fMax;

    const SDL_Rect texRect = boundingRect();

    const auto r = p.renderer();
    const auto shadow = std::make_shared<eTexture>();

    {
        shadow->create(r, texRect.w, texRect.h);
        ePainter sp(r);
        sp.translate(-texRect.x, -texRect.y);
        const auto holder = shadow->createTargetHolder(r);
        for(int g = 0; g < gMax; g++) {
            const int ppMax = mModel->nParts(g);
            for(int pp = 0; pp < ppMax; pp++) {
                const auto tex = mModel->get(mAnim, g, pp, mDir, frame);
                sp.drawTexture(0, 0, tex);
            }
        }
    }

    p.save();
    const auto& offset = mModel->animOffset(mAnim);
    p.translate(offset.fX, offset.fY);

    {
        const float skew = 0.5f;
        const float scaleY = 0.5f;

        const float x = p.x() - shadow->width();
        const float y = p.y() - shadow->height()*scaleY;

        const float w = shadow->width();
        const float h = shadow->height() * scaleY;

        // Bottom horizontal shift caused by skew
        const float skewOffset = h * skew;

        SDL_Vertex verts[4];

        // Top-left
        verts[0].position = { x, y };
        verts[0].tex_coord = { 0.0f, 0.0f };
        verts[0].color = { 0.f, 0.f, 0.f, 0.5f };

        // Top-right
        verts[1].position = { x + w, y };
        verts[1].tex_coord = { 1.0f, 0.0f };
        verts[1].color = { 0.f, 0.f, 0.f, 0.5f };

        // Bottom-right (skewed)
        verts[2].position = { x + w + skewOffset, y + h };
        verts[2].tex_coord = { 1.0f, 1.0f };
        verts[2].color = { 0.f, 0.f, 0.f, 0.5f };

        // Bottom-left (skewed)
        verts[3].position = { x + skewOffset, y + h };
        verts[3].tex_coord = { 0.0f, 1.0f };
        verts[3].color = { 0.f, 0.f, 0.f, 0.5f };

        const int indices[6] = { 0, 1, 2, 0, 2, 3 };

        SDL_RenderGeometry(p.renderer(), shadow->tex(), verts, 4, indices, 6);
    }

    for(int g = 0; g < gMax; g++) {
        const int ppMax = mModel->nParts(g);
        for(int pp = 0; pp < ppMax; pp++) {
            const auto& tex = mModel->get(mAnim, g, pp, mDir, frame);
            p.drawTexture(0, 0, tex);
        }
    }

    p.restore();
    // p.fillRect(SDL_Rect{-2, -2, 4, 4}, SDL_Color{255, 0, 0, 255});
}

void eCharUnitModel::setAngle(const double a) {
    const int dirs = mModel->nDirs();
    const double ainc = 360./dirs;
    const int dir = std::round(a/ainc) - 2*dirs/16;
    setDirection((dirs + dir) % dirs);
}

void eCharUnitModel::generatePreview(SDL_Renderer* const r) {
    const std::string dir = "/home/ailuropoda/.eSlayer/tmp/preview/";
    for(int anim = 0; anim < mModel->nAnims(); anim++) {
        mAnim = anim;
        mFrame = mModel->nFrames(anim) - 1;
        int id = 0;
        while(std::filesystem::exists(dir + std::to_string(id) + ".png")) {
            id++;
        }
        const std::string path = dir + std::to_string(/*id*/anim) + ".png";

        const int dim = 300;

        const auto tex = std::make_shared<eTexture>();
        tex->create(r, dim, dim);
        {
            const auto holder = tex->createTargetHolder(r);
            ePainter p(r);
            p.translate(dim/2, dim/2);
            drawBase(p);
        }
        tex->save(r, path);
    }
}

void eCharUnitModel::setAnimation(const int a, const int id) {
    if(id <= mAnimId) return;
    mAnimId = id;
    setAnimation(a);
}

void eCharUnitModel::setAnimation(const int a) {
    if(a < 0 || a >= mModel->nAnims()) {
        eExceptions::logError("Animation id " +
                              std::to_string(a) +
                              " out of range!");
        return;
    }
    if(mAnim != a) {
        mAnim = a;
        mFrame = 0.;
        const auto& data = mModel->data();
        mClamp = data.animClamp(a);
    }
}

void eCharUnitModel::setDirection(const int d) {
    if(d < 0 || d >= mModel->nDirs()) {
        eExceptions::logError("Direction id " +
                              std::to_string(d) +
                              " out of range!");
        return;
    }
    mDir = d;
}
