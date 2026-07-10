#ifndef ESTATSWIDGET_H
#define ESTATSWIDGET_H

#include "ebgwidget.h"

struct eStats;
struct eAttributes;
struct eEquipment;
class eStatLabel;
class eStatWidget;
class eAttrIncButton;

class eStatsWidgetBase : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const std::string& name,
                    eStats& stats,
                    const eEquipment& eq,
                    eAttributes& attrs,
                    const bool merc = false);
protected:
    void paintEvent(ePainter& p) override;
private:
    eStats* mStats = nullptr;
    const eEquipment* mEq = nullptr;
    eAttributes* mAttrs = nullptr;

    void updateStats();

    eStatLabel* mName = nullptr;

    eStatLabel* mLevel = nullptr;
    eStatLabel* mExp = nullptr;
    eStatLabel* mNextLevel = nullptr;

    eStatWidget* mStrength = nullptr;
    eAttrIncButton* mStrIncButton = nullptr;
    eStatWidget* mDexterity = nullptr;
    eAttrIncButton* mDexIncButton = nullptr;
    eStatWidget* mVitality = nullptr;
    eAttrIncButton* mVitIncButton = nullptr;
    eStatWidget* mEnergy = nullptr;
    eAttrIncButton* mEneIncButton = nullptr;

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

    eStatWidget* mStatPointsRem = nullptr;

};

class eStatsWidget : public eBgWidget {
public:
    using eBgWidget::eBgWidget;

    void initialize(const std::string& name,
                    eStats& stats,
                    const eEquipment& eq,
                    eAttributes& attrs);};

#endif // ESTATSWIDGET_H
