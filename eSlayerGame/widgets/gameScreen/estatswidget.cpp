#include "estatswidget.h"

#include "../../elanguage.h"
#include "../../names/eskillnames.h"
#include "../elabel.h"
#include "../ebuttonbase.h"
#include "../../emainwindow.h"

#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/estats.h>
#include <eSlayerHelpers/eattributes.h>

#include <iostream>
#include <sstream>
#include <cmath>

class eAttrIncButton : public eButtonBase {
public:
    eAttrIncButton(eMainWindow * const window) :
        eButtonBase(window) {
        setFontColor(eFontColor::white);
        setText("+");
        setNoPadding();
        setHugeFontSize();
        const auto& res = resolution();
        const auto mult = res.multiplier();
        const int dim = 35*mult;
        resize(dim, dim);
    }

    void paintEvent(ePainter& p) {
        if(enabled()) {
            setFontColor(eFontColor::white);
            if(hovered()) {
                const SDL_Color white{255, 255, 255, 255};
                p.drawRect(rect(), white, lineWidth());
            }
        } else {
            setFontColor(eFontColor::gray);
        }
        return eButtonBase::paintEvent(p);
    }
};

class eStatLabel : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const int width) {
        setNoPadding();
        const auto& res = resolution();
        const int fontSize = res.tinyFontSize();
        mFont = eFonts::textFont(fontSize);

        const auto mult = res.multiplier();
        resize(width*mult, 35*mult);
    }

    void setValues(const std::vector<std::string>& values) {
        removeChildren();
        for(const auto& l : values) {
            const auto label = new eLabel(window());
            label->setNoPadding();
            label->setFont(mFont);
            label->setText(l);
            label->fitContent();
            addWidget(label);
            label->align(eAlignment::hcenter);
        }
        layoutVertically();
    }
protected:
    void paintEvent(ePainter& p) override {
        const auto& res = resolution();
        const int lineWidth = res.lineWidth();
        p.drawRect(rect(), SDL_Color{255, 255, 255, 255}, lineWidth);
    }
private:
    eFont mFont;
};

class eStatWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const int nValues = 1) {
        setNoPadding();

        const auto& res = resolution();

        mNames = new eStatLabel(window());
        mNames->initialize(100);
        addWidget(mNames);

        for(int i = 0; i < nValues; i++) {
            const auto values = new eStatLabel(window());
            values->initialize(50);
            addWidget(values);
            mValues.emplace_back(values);
        }

        const int p = res.smallPadding();
        stackHorizontally(p);

        fitContent();
    }

    void setText(const std::vector<std::string>& names,
                 const std::vector<std::string>& values) {
        mNames->setValues(names);
        int id = 0;
        for(const auto v : mValues) {
            std::vector<std::string> vvalues;
            for(int i = 0; i < values.size()/mValues.size(); i++) {
                vvalues.emplace_back(values[id++]);
            }
            v->setValues(vvalues);
        }
    }

    void setText(const std::vector<std::string>& names,
                 const std::vector<float>& values) {
        std::vector<std::string> valueStrs;
        for(const auto v : values) {
            std::ostringstream oss;
            oss << std::round(v);
            valueStrs.emplace_back(oss.str());
        }
        setText(names, valueStrs);
    }

    void setText(const std::vector<std::string>& names,
                 const std::vector<std::pair<float, float>>& values) {
        std::vector<std::string> valueStrs;
        for(const auto& v : values) {
            const float min = v.first;
            const float max = v.second;
            auto& str = valueStrs.emplace_back();
            {
                std::ostringstream oss;
                oss << std::round(min);
                str += oss.str();
            }
            {
                std::ostringstream oss;
                oss << std::round(max);
                str += "-" + oss.str();
            }
        }
        setText(names, valueStrs);
    }
protected:
    void paintEvent(ePainter& p) override {
        const auto& res = resolution();
        const int lineWidth = res.lineWidth();
        p.drawRect(rect(), SDL_Color{255, 255, 255, 255}, lineWidth);
    }
private:
    eStatLabel* mNames = nullptr;
    std::vector<eStatLabel*> mValues;
};

void eStatsWidget::initialize(const std::string& name,
                              eStats& stats,
                              const eEquipment& eq,
                              eAttributes& attrs) {
    mStats = &stats;
    mEq = &eq;
    mAttrs = &attrs;

    const auto& res = resolution();
    const int hp = res.largePadding();
    const int sp = res.tinyPadding();

    const auto innerW = new eWidget(window());
    innerW->setNoPadding();

    mName = new eStatLabel(window());
    mName->initialize(300);
    mName->setValues({name});

    const auto expW = new eWidget(window());
    expW->setNoPadding();

    mLevel = new eStatLabel(window());
    mLevel->initialize(100);

    mExp = new eStatLabel(window());
    mExp->initialize(200);

    mNextLevel = new eStatLabel(window());
    mNextLevel->initialize(200);

    expW->addWidget(mLevel);
    expW->addWidget(mExp);
    expW->addWidget(mNextLevel);
    expW->stackHorizontally(hp);
    expW->fitContent();

    const auto strW = new eWidget(window());
    strW->setNoPadding();

    mStrength = new eStatWidget(window());
    mStrength->initialize();

    const auto incAttr = [this](uint16_t eAttributes::*attr) {
        const auto w = window();
        uint16_t inc = 1;
        if(w->shiftPressed()) {
            inc = 10;
        }
        inc = std::clamp(inc, uint16_t(0), mAttrs->fStatPoints);
        mAttrs->*attr += inc;
        mAttrs->fStatPoints -= inc;

        mStats->calculate(*mAttrs, *mEq);
    };

    mStrIncButton = new eAttrIncButton(window());
    mStrIncButton->setPressAction([incAttr]() {
        incAttr(&eAttributes::fStrength);
    });

    const auto dmgWidget = new eWidget(window());
    dmgWidget->setNoPadding();

    mLeftSkillDmg = new eStatWidget(window());
    mLeftSkillDmg->initialize();
    mRightSkillDmg = new eStatWidget(window());
    mRightSkillDmg->initialize();

    dmgWidget->addWidget(mLeftSkillDmg);
    dmgWidget->addWidget(mRightSkillDmg);
    dmgWidget->stackVertically(sp);
    dmgWidget->fitContent();

    strW->addWidget(mStrength);
    strW->addWidget(mStrIncButton);
    strW->addWidget(dmgWidget);
    strW->stackHorizontally(hp);
    strW->fitContent();

    const auto dexW = new eWidget(window());
    dexW->setNoPadding();

    mDexterity = new eStatWidget(window());
    mDexterity->initialize();

    mDexIncButton = new eAttrIncButton(window());
    mDexIncButton->setPressAction([incAttr]() {
        incAttr(&eAttributes::fDexterity);
    });

    const auto arDefenseWidget = new eWidget(window());
    arDefenseWidget->setNoPadding();

    mLeftSkillAR = new eStatWidget(window());
    mLeftSkillAR->initialize();
    mRightSkillAR = new eStatWidget(window());
    mRightSkillAR->initialize();

    mDefense = new eStatWidget(window());
    mDefense->initialize();

    arDefenseWidget->addWidget(mLeftSkillAR);
    arDefenseWidget->addWidget(mRightSkillAR);
    arDefenseWidget->addWidget(mDefense);
    arDefenseWidget->stackVertically(sp);
    arDefenseWidget->fitContent();

    dexW->addWidget(mDexterity);
    dexW->addWidget(mDexIncButton);
    dexW->addWidget(arDefenseWidget);
    dexW->stackHorizontally(hp);
    dexW->fitContent();

    const auto vitW = new eWidget(window());
    vitW->setNoPadding();

    mVitality = new eStatWidget(window());
    mVitality->initialize();

    mVitIncButton = new eAttrIncButton(window());
    mVitIncButton->setPressAction([incAttr]() {
        incAttr(&eAttributes::fVitality);
    });

    const auto staminaLifeWidget = new eWidget(window());
    staminaLifeWidget->setNoPadding();

    mStamina = new eStatWidget(window());
    mStamina->initialize(2);
    mLife = new eStatWidget(window());
    mLife->initialize(2);

    staminaLifeWidget->addWidget(mStamina);
    staminaLifeWidget->addWidget(mLife);
    staminaLifeWidget->stackVertically(sp);
    staminaLifeWidget->fitContent();

    vitW->addWidget(mVitality);
    vitW->addWidget(mVitIncButton);
    vitW->addWidget(staminaLifeWidget);
    vitW->stackHorizontally(hp);
    vitW->fitContent();

    const auto eneW = new eWidget(window());
    eneW->setNoPadding();

    mEnergy = new eStatWidget(window());
    mEnergy->initialize();

    mEneIncButton = new eAttrIncButton(window());
    mEneIncButton->setPressAction([incAttr]() {
        incAttr(&eAttributes::fEnergy);
    });

    mMana = new eStatWidget(window());
    mMana->initialize(2);

    eneW->addWidget(mEnergy);
    eneW->addWidget(mEneIncButton);
    eneW->addWidget(mMana);
    eneW->stackHorizontally(hp);
    eneW->fitContent();

    const auto resStatPointsWidget = new eWidget(window());
    resStatPointsWidget->setNoPadding();

    const auto resWidget = new eWidget(window());
    resWidget->setNoPadding();

    mFireResistance = new eStatWidget(window());
    mFireResistance->initialize();
    mColdResistance = new eStatWidget(window());
    mColdResistance->initialize();
    mLightningResistance = new eStatWidget(window());
    mLightningResistance->initialize();
    mPoisonResistance = new eStatWidget(window());
    mPoisonResistance->initialize();

    resWidget->addWidget(mFireResistance);
    resWidget->addWidget(mColdResistance);
    resWidget->addWidget(mLightningResistance);
    resWidget->addWidget(mPoisonResistance);
    resWidget->stackVertically(sp);
    resWidget->fitContent();

    mStatPointsRem = new eStatWidget(window());
    mStatPointsRem->initialize();

    resStatPointsWidget->addWidget(resWidget);
    resStatPointsWidget->addWidget(mStatPointsRem);
    resStatPointsWidget->stackHorizontally(hp);
    resStatPointsWidget->fitContent();

    innerW->addWidget(mName);
    innerW->addWidget(expW);
    innerW->addWidget(strW);
    innerW->addWidget(dexW);
    innerW->addWidget(vitW);
    innerW->addWidget(eneW);
    innerW->addWidget(resStatPointsWidget);

    innerW->stackVertically(hp);
    innerW->fitContent();

    addWidget(innerW);
    innerW->align(eAlignment::center);
}

bool eStatsWidget::mousePressEvent(const eMouseEvent& e) {
    return true;
}

void eStatsWidget::paintEvent(ePainter& p) {
    updateStats();
    p.fillRect(rect(), SDL_Color{0, 0, 0, 255});
    const auto& res = resolution();
    const int lineWidth = res.lineWidth();
    p.drawRect(rect(), SDL_Color{255, 255, 255, 255}, lineWidth);
}

void eStatsWidget::updateStats() {
    mLevel->setValues({eLanguage::text(11, 16),
                       std::to_string(mAttrs->fLevel)});
    mExp->setValues({eLanguage::text(11, 17),
                     std::to_string(int(mAttrs->fExp))});
    mNextLevel->setValues({eLanguage::text(11, 18),
                           std::to_string(mAttrs->nextLevelExp())});

    mStrength->setText({eLanguage::text(11, 0)},
                       std::vector<float>{mStats->fStrength});
    mDexterity->setText({eLanguage::text(11, 1)},
                       std::vector<float>{mStats->fDexterity});
    mVitality->setText({eLanguage::text(11, 2)},
                       std::vector<float>{mStats->fVitality});
    mEnergy->setText({eLanguage::text(11, 3)},
                      std::vector<float>{mStats->fEnergy});

    const auto& leftSkillStats = mStats->leftSkill();
    const int leftSkillId = leftSkillStats.fSkillId;
    const auto leftSkillName = eSkillNames::name(leftSkillId);
    mLeftSkillDmg->setText({leftSkillName, eLanguage::text(11, 4)},
                           {std::pair<float, float>{leftSkillStats.fDamageMinLW.total(),
                                                    leftSkillStats.fDamageMaxLW.total()},
                            std::pair<float, float>{leftSkillStats.fDamageMinRW.total(),
                                                    leftSkillStats.fDamageMaxRW.total()}});

    const auto& rightSkillStats = mStats->rightSkill();
    const int rightSkillId = rightSkillStats.fSkillId;
    const auto rightSkillName = eSkillNames::name(rightSkillId);
    mRightSkillDmg->setText({rightSkillName, eLanguage::text(11, 4)},
                           {std::pair<float, float>{rightSkillStats.fDamageMinLW.total(),
                                                     rightSkillStats.fDamageMaxLW.total()},
                            std::pair<float, float>{rightSkillStats.fDamageMinRW.total(),
                                                     rightSkillStats.fDamageMaxRW.total()}});

    const auto& leftSkill = eSkills::sSkills.get(leftSkillId);
    const auto leftSkillType = leftSkill.fType;
    const bool alwaysHitL = leftSkillType == eSkillType::missile ||
                            leftSkillType == eSkillType::wall;
    if(alwaysHitL) {
        mLeftSkillAR->setText(std::vector<std::string>{},
                              std::vector<std::string>{});
    } else {
        mLeftSkillAR->setText({leftSkillName, eLanguage::text(11, 5)},
                              {leftSkillStats.fAttackRatingLW,
                               leftSkillStats.fAttackRatingRW});
    }

    const auto& rightSkill = eSkills::sSkills.get(rightSkillId);
    const auto rightSkillType = rightSkill.fType;
    const bool alwaysHitR = rightSkillType == eSkillType::missile ||
                            rightSkillType == eSkillType::wall;
    if(alwaysHitR) {
        mRightSkillAR->setText(std::vector<std::string>{},
                               std::vector<std::string>{});
    } else {
        mRightSkillAR->setText({rightSkillName, eLanguage::text(11, 5)},
                              {rightSkillStats.fAttackRatingLW,
                               rightSkillStats.fAttackRatingRW});
    }

    mDefense->setText({eLanguage::text(11, 6)},
                      std::vector<float>{mStats->fDefense});

    mStamina->setText({eLanguage::text(11, 7)},
                      {mStats->fStamina, mStats->fStamina});
    mLife->setText({eLanguage::text(11, 8)},
                   {mStats->fMaxHealth, mStats->fHealthF});
    mMana->setText({eLanguage::text(11, 9)},
                    {mStats->fMaxMana, mStats->fManaF});

    mFireResistance->setText({eLanguage::text(11, 10),
                              eLanguage::text(11, 14)},
                             std::vector<float>{100*mStats->fFireResistance});
    mColdResistance->setText({eLanguage::text(11, 11),
                              eLanguage::text(11, 14)},
                             std::vector<float>{100*mStats->fColdResistance});
    mLightningResistance->setText({eLanguage::text(11, 12),
                                   eLanguage::text(11, 14)},
                                  std::vector<float>{100*mStats->fLightningResistance});
    mPoisonResistance->setText({eLanguage::text(11, 13),
                                eLanguage::text(11, 14)},
                               std::vector<float>{100*mStats->fPoisonResistance});

    mStatPointsRem->setText({eLanguage::text(11, 15)},
                            std::vector<float>{float(mAttrs->fStatPoints)});

    mStrIncButton->setEnabled(mAttrs->fStatPoints > 0);
    mDexIncButton->setEnabled(mAttrs->fStatPoints > 0);
    mVitIncButton->setEnabled(mAttrs->fStatPoints > 0);
    mEneIncButton->setEnabled(mAttrs->fStatPoints > 0);
}
