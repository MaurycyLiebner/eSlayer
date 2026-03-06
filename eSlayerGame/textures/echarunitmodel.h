#ifndef ECHARUNITMODEL_H
#define ECHARUNITMODEL_H

#include "echarmodel.h"

class ePainter;

class eCharUnitModel {
public:
    eCharUnitModel();

    void setCharModel(const eCharModel& model);

    void incFrame(const double by);
    void draw(ePainter& p) const;

    void setAnimation(const int a, const int id);
    void setAnimation(const int a);
    void setDirection(const int d);
    void setAngle(const double a);
private:
    int mAnim;
    int mAnimId = -1;
    double mFrame = 0.;
    int mDir;
    eCharModel mModel;
};

#endif // ECHARUNITMODEL_H
