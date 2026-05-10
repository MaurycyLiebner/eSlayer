#ifndef ECHARUNITMODEL_H
#define ECHARUNITMODEL_H

#include "echarmodel.h"
#include "epaintcall.h"

class eGamePainter;
class ePainter;
class eResolution;

class eCharUnitModel {
public:
    eCharUnitModel();

    void setCharModel(const std::shared_ptr<eCharModel>& model);
    const eCharModel& model() const { return *mModel; }

    eTextureKey key() const;

    SDL_Rect offsetBoundingRect() const;

    void incFrame(const float by);
    void draw(eGamePainter& p,
              const eResolution& res,
              const bool highlight = false,
              const SDL_Color& colorMod = SDL_Color{0, 0, 0, 0}) const;
    void draw(ePainter& p,
              const eResolution& res,
              const bool highlight = false,
              const SDL_Color& colorMod = SDL_Color{0, 0, 0, 0}) const;
    ePaintCall paintCall(SDL_Renderer* const r) const;

    bool aggressive() const { return mAggressive; }
    void setAggressive(const bool a) { mAggressive = a; }

    void setAnimationSpeed(const float speed);

    void setAnimation(const int a, const int id,
                      const float speed);
    void setAnimation(const int a,
                      const float speed);

    void setDirection(const int d);
    void setAngle(const float a);

    void generatePreview(const eResolution& res,
                         SDL_Renderer* const r);
private:
    void draw(ePainter& p,
              const eResolution& res,
              const bool highlight,
              const std::shared_ptr<eTexture>& tex,
              const SDL_Rect& texRect,
              const SDL_Color& colorMod) const;

    int mAnim = 0;
    int mAnimId = -1;
    float mAnimSpeed = 1.f;
    float mFrame = 0.f;
    int mClampId = -1;
    bool mAggressive = false;
    int mDir = 0;
    std::shared_ptr<eCharModel> mModel;
};

#endif // ECHARUNITMODEL_H
