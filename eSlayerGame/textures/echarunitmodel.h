#ifndef ECHARUNITMODEL_H
#define ECHARUNITMODEL_H

#include "echarmodel.h"

class ePainter;

class eCharUnitModel {
public:
    eCharUnitModel();

    void setCharModel(const eCharModel& model);

    void draw(ePainter& p, const int frame) const;

    void setAnimation(const int a) { mAnim = a; }
    void setDirection(const int d) { mDir = d; }
    void setAngle(const double a);
private:
    int mAnim;
    int mDir;
    eCharModel mModel;
};

#endif // ECHARUNITMODEL_H
