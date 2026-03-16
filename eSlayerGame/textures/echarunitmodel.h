#ifndef ECHARUNITMODEL_H
#define ECHARUNITMODEL_H

#include "echarmodel.h"

class eGamePainter;
class ePainter;

class eCharUnitModel {
public:
    eCharUnitModel();

    void setCharModel(const std::shared_ptr<eCharModel>& model);
    const eCharModel& model() const { return *mModel; }

    SDL_Rect boundingRect() const;
    SDL_Rect offsetBoundingRect() const;

    void incFrame(const float by);
    void draw(eGamePainter& p, const bool highligh = false) const;
    void drawBase(ePainter& p) const;

    bool aggressive() const { return mAggressive; }
    void setAggressive(const bool a) { mAggressive = a; }

    void setAnimationSpeed(const float speed);

    void setAnimation(const int a, const int id,
                      const float speed);
    void setAnimation(const int a,
                      const float speed);

    void setDirection(const int d);
    void setAngle(const float a);

    void generatePreview(SDL_Renderer* const r);
private:
    int mAnim = 0;
    int mAnimId = -1;
    float mAnimSpeed = 1.f;
    float mFrame = 0.f;
    std::string mClamp;
    bool mAggressive = false;
    int mDir = 0;
    std::shared_ptr<eCharModel> mModel;
};

#endif // ECHARUNITMODEL_H
