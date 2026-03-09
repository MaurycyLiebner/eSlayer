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

    void incFrame(const double by);
    void draw(eGamePainter& p, const bool highligh = false) const;
    void drawBase(ePainter& p) const;

    bool aggressive() const { return false; }

    void setAnimation(const int a, const int id);
    void setAnimation(const int a);

    void setDirection(const int d);
    void setAngle(const double a);

    void generatePreview(SDL_Renderer* const r);
private:
    int mAnim;
    int mAnimId = -1;
    double mFrame = 0.;
    bool mClamp = false;
    int mDir;
    std::shared_ptr<eCharModel> mModel;
};

#endif // ECHARUNITMODEL_H
