#include "echarunitmodel.h"

#include "../widgets/epainter.h"
#include "echarmodel.h"
#include "echartextures.h"

#include <eSlayerHelpers/eexceptions.h>

#include <cmath>
#include <filesystem>

eCharUnitModel::eCharUnitModel() {}

void eCharUnitModel::setCharModel(const std::shared_ptr<eCharModel>& model) {
    mModel = model;
}

eTextureKey eCharUnitModel::key() const {
    const int fMax = mModel->nFrames(mAnim);
    const int frame = int(std::round(mFrame)) % fMax;
    return eTextureKey{mAnim, frame, mDir};
}

SDL_Rect eCharUnitModel::offsetBoundingRect() const {
    SDL_Rect result = mModel->requestBoundingRect(key());
    const auto& offset = mModel->animOffset(mAnim);
    result.x += offset.fX;
    result.y += offset.fY;
    return result;
}

void eCharUnitModel::incFrame(const float by) {
    mFrame += by*mAnimSpeed;
    if(mClampId != -1) {
        const int fMax = mModel->nFrames(mAnim);
        if(int(std::round(mFrame)) >= fMax) {
            setAnimation(mClampId, 1.f);
        }
    }
}

void eCharUnitModel::draw(ePainter& p, const bool highlight) const {
    const auto r = p.renderer();
    const auto tex = mModel->requestTexture(r, key());
    const auto texRect = mModel->requestBoundingRect(key());

    p.save();
    const auto& offset = mModel->animOffset(mAnim);
    p.translate(offset.fX, offset.fY);

    {
        const float skew = 0.5f;
        const float scaleY = 0.5f;

        const float x = p.x() - tex->width();
        const float y = p.y() - tex->height()*scaleY;

        const float w = tex->width();
        const float h = tex->height() * scaleY;

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

        SDL_RenderGeometry(p.renderer(), tex->tex(), verts, 4, indices, 6);
    }

    p.drawTexture(texRect.x, texRect.y, tex);
    if(highlight) {
        tex->setBlendMode(SDL_BLENDMODE_ADD);
        tex->setAlpha(125);
        p.drawTexture(texRect.x, texRect.y, tex);
        tex->setBlendMode(SDL_BLENDMODE_BLEND);
        tex->clearAlphaMod();
    }
    p.restore();
    // p.fillRect(SDL_Rect{-2, -2, 4, 4}, SDL_Color{255, 0, 0, 255});
}

void eCharUnitModel::setAnimationSpeed(const float speed) {
    mAnimSpeed = speed;
}

void eCharUnitModel::setAngle(const float a) {
    const int dirs = mModel->nDirs();
    const float ainc = 360.f/dirs;
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
            draw(p, true);
        }
        tex->save(r, path);
    }
}

void eCharUnitModel::setAnimation(const int a, const int id,
                                  const float speed) {
    if(id <= mAnimId) return;
    mAnimId = id;
    setAnimation(a, speed);
}

void eCharUnitModel::setAnimation(const int a,
                                  const float speed) {
    if(a < 0 || a >= mModel->nAnims()) {
        eExceptions::logError("Animation id " +
                              std::to_string(a) +
                              " out of range!");
        return;
    }
    if(mAnim != a) {
        mAnim = a;
        mFrame = 0.f;
        const auto& data = mModel->data();
        mClampId = data.animClamp(a);
        mAnimSpeed = speed;
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
