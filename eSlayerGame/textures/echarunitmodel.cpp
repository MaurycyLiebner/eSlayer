#include "echarunitmodel.h"

#include "../widgets/epainter.h"
#include "echarmodel.h"
#include "echartextures.h"

#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerHelpers/eexceptions.h>

#include <cmath>
#include <filesystem>

eCharUnitModel::eCharUnitModel() {}

void eCharUnitModel::setCharModel(const std::shared_ptr<eCharModel>& model) {
    mModel = model;
}

eTextureKey eCharUnitModel::key() const {
    const int fMax = mModel->nFrames(mAnim);
    const int frame = fMax == 0 ? -1 : (eCharUnitModel::frame() % fMax);
    return eTextureKey{mAnim, frame, mDir};
}

SDL_Rect eCharUnitModel::requestBoundingRect() const {
    const auto result = mModel->requestBoundingRect(key());
    return result;
}

void eCharUnitModel::incFrame(const float by) {
    mFrame += by*mAnimSpeed;
    if(mClampId != -1) {
        const int fMax = mModel->nFrames(mAnim);
        if(frame() >= fMax) {
            setAnimation(mClampId, 1.f);
        }
    }
}

int eCharUnitModel::frame() const {
    return std::round(mFrame);
}

std::shared_ptr<eTexture> eCharUnitModel::requestTexture(
    SDL_Renderer* const r) const {
    const auto key = eCharUnitModel::key();
    if(key.fFrame == -1) return nullptr;
    const auto tex = mModel->requestTexture(r, key);
    return tex;
}

void eCharUnitModel::draw(ePainter& p) const {
    const auto key = eCharUnitModel::key();
    if(key.fFrame == -1) return;
    const auto r = p.renderer();
    const auto tex = mModel->requestTexture(r, key);
    const auto texRect = mModel->requestBoundingRect(key);
    p.drawTexture(texRect.x, texRect.y, tex);

    tex->setBlendMode(SDL_BLENDMODE_ADD);
    tex->setAlpha(128);
    p.drawTexture(texRect.x, texRect.y, tex);
    tex->clearAlphaMod();
    tex->setBlendMode(SDL_BLENDMODE_BLEND);
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

void eCharUnitModel::generatePreview(
    const eResolution& res, SDL_Renderer* const r) {
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
            draw(p);
        }
        tex->save(r, path);
    }
}

void eCharUnitModel::setAnimation(
    const int a, const eAnimId& id,
    const float speed) {
    if(!eAnimId::isAfter(id, mAnimId)) return;
    mAnimId = id;
    setAnimation(a, speed);
}

void eCharUnitModel::setAnimation(
    const int a, const float speed) {
    if(a == sFleshExplAnim ||
       a == sIceExplAnim) {
        mAnim = a;
        mFrame = 0.f;
        mClampId = -1;
        mAnimSpeed = speed;
        return;
    }
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
