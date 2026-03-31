#ifndef ESTATSWIDGET_H
#define ESTATSWIDGET_H

#include "../ewidget.h"

struct eStats;
class eStatWidget;

class eStatsWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const eStats* const stats);
protected:
    void paintEvent(ePainter& p) override;
private:
    const eStats* mStats = nullptr;

    void updateStats();

    eStatWidget* mStrength = nullptr;
    eStatWidget* mDexterity = nullptr;
    eStatWidget* mVitality = nullptr;
    eStatWidget* mEnergy = nullptr;

    eStatWidget* mLeftSkillDmg = nullptr;
    eStatWidget* mRightSkillDmg = nullptr;

    eStatWidget* mLeftSkillAR = nullptr;
    eStatWidget* mRightSkillAR = nullptr;

    eStatWidget* mDefense = nullptr;

    eStatWidget* mStamina = nullptr;
    eStatWidget* mLife = nullptr;
    eStatWidget* mMana = nullptr;

    eStatWidget* mFireResistance = nullptr;
    eStatWidget* mColdResistance = nullptr;
    eStatWidget* mLightningResistance = nullptr;
    eStatWidget* mPoisonResistance = nullptr;
};

#endif // ESTATSWIDGET_H
