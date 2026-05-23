#include "estatswidget.h"

#include "../../elanguage.h"
#include "../../names/eskillnames.h"
#include "../elabel.h"
#include "../ebuttonbase.h"
#include "../../emainwindow.h"
#include "egamewidget.h"
#include "../../textures/euitextures.h"

#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/estats.h>
#include <eSlayerHelpers/eattributes.h>
#include <eSlayerHelpers/eweaponchoice.h>

#include <iostream>
#include <sstream>
#include <cmath>

class eAttrIncButton : public eButtonBase {
public:
    eAttrIncButton(eMainWindow * const window) :
        eButtonBase(window) {
        setNoPadding();
        const auto& tex = eUITextures::sStatsPlusButton;
        setTexture(tex);
        fitContent();
    }

    void paintEvent(ePainter& p) {
        if(enabled()) {
            if(hovered()) {
                const auto& tex = eUITextures::sStatsPlusButtonHovered;
                p.drawTexture(0, 0, tex);
            } else {
                const auto& tex = eUITextures::sStatsPlusButton;
                p.drawTexture(0, 0, tex);
            }
        }
    }
};

class eStatLabel : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const int width,
                    const bool main) {
        mMain = main;
        setNoPadding();
        const auto& res = resolution();
        const int fontSize = res.tinyFontSize();
        mFont = eFonts::textFont(fontSize);

        const auto mult = res.multiplier();
        const auto& coll = eUITextures::sStats;
        const auto& first = coll.getTexture(0);
        const int h = first->height();
        resize(width*mult, h);
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
        const auto& coll = eUITextures::sStats;
        const auto& first = coll.getTexture(0);
        const int dim = first->width();
        const int xMax = width()/dim;
        int id = 0;
        for(int x = 0; x <= xMax; x++) {
            if(x == 0) {
                id = mMain ? 0 : 3;
            } else if(x == xMax) {
                id = 2;
            } else {
                id = 1;
            }
            const auto& tex = coll.getTexture(id);
            int px = x*dim;
            if(x == xMax) px = width() - dim;
            p.drawTexture(px, 0, tex);
        }
    }
private:
    bool mMain = true;
    eFont mFont;
};

class eStatWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const int nValues = 1) {
        setNoPadding();

        const auto& res = resolution();

        mNames = new eStatLabel(window());
        mNames->initialize(100, true);
        addWidget(mNames);

        for(int i = 0; i < nValues; i++) {
            const auto values = new eStatLabel(window());
            values->initialize(65, false);
            addWidget(values);
            mValues.emplace_back(values);
        }

        stackHorizontally(0);

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

    const auto topW = new eWidget(window());
    topW->setNoPadding();

    mName = new eStatLabel(window());
    mName->initialize(150, true);
    mName->setValues({name});

    mLevel = new eStatLabel(window());
    mLevel->initialize(75, true);

    mExp = new eStatLabel(window());
    mExp->initialize(100, true);

    mNextLevel = new eStatLabel(window());
    mNextLevel->initialize(100, true);

    topW->addWidget(mName);
    topW->addWidget(mLevel);
    topW->addWidget(mExp);
    topW->addWidget(mNextLevel);
    topW->stackHorizontally(hp);
    topW->fitContent();

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
        if(inc) {
            mAttrs->*attr += inc;
            mAttrs->fStatPoints -= inc;

            eGameWidget::sSendAttributesChanged();
        }
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

    innerW->addWidget(topW);
    innerW->addWidget(strW);
    innerW->addWidget(dexW);
    innerW->addWidget(vitW);
    innerW->addWidget(eneW);
    innerW->addWidget(resStatPointsWidget);

    innerW->stackVertically(hp);
    innerW->fitContent();

    setup(innerW);
}

bool eStatsWidget::mousePressEvent(const eMouseEvent& e) {
    return true;
}

void eStatsWidget::paintEvent(ePainter& p) {
    updateStats();
    eBgWidget::paintEvent(p);
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

    const auto handleSkill = [&](const eSkillStats& skillStats,
                                 const eSkillChoice schoice,
                                 eStatWidget* const skillDmg,
                                 eStatWidget* const skillAR) {
        const int skillId = skillStats.fSkillId;
        const auto skillName = eSkillNames::name(skillId);
        const auto& skill = eSkills::sSkills.get(skillId);
        const auto skillType = skill.fType;
        bool lw = false;
        bool rw = false;
        switch(skillType) {
        case eSkillType::attack:
        case eSkillType::throw_:
        case eSkillType::smite:
        case eSkillType::kick:
        case eSkillType::shoot:
            lw = mStats->canUseSkill(static_cast<int>(schoice),
                                     eWeaponChoice::left);
            rw = mStats->canUseSkill(static_cast<int>(schoice),
                                     eWeaponChoice::right);
            break;
        case eSkillType::missile:
        case eSkillType::wall:
        case eSkillType::nova:
        case eSkillType::boostCurse:
            lw = mStats->canUseSkill(static_cast<int>(schoice),
                                     eWeaponChoice::left);
            break;
        default:
            break;
        }

        std::vector<std::pair<float, float>> dmg;
        if(lw) {
            dmg.emplace_back(skillStats.fDamageMinLW.total(),
                             skillStats.fDamageMaxLW.total());
        }
        if(rw) {
            dmg.emplace_back(skillStats.fDamageMinRW.total(),
                             skillStats.fDamageMaxRW.total());
        }

        skillDmg->setText({skillName, eLanguage::text(11, 4)}, dmg);

        const bool noARL = skillType == eSkillType::missile ||
                           skillType == eSkillType::wall ||
                           skillType == eSkillType::nova ||
                           skillType == eSkillType::summon ||
                           skillType == eSkillType::aura ||
                           skillType == eSkillType::passive ||
                           skillType == eSkillType::boostCurse;
        if(noARL) {
            skillAR->setText(std::vector<std::string>{},
                             std::vector<std::string>{});
        } else {
            std::vector<float> AR;
            if(lw) {
                AR.emplace_back(skillStats.fAttackRatingLW);
            }
            if(rw) {
                AR.emplace_back(skillStats.fAttackRatingRW);
            }
            skillAR->setText({skillName, eLanguage::text(11, 5)}, AR);
        }
    };

    handleSkill(mStats->leftSkill(),
                eSkillChoice::left,
                mLeftSkillDmg,
                mLeftSkillAR);
    handleSkill(mStats->rightSkill(),
                eSkillChoice::right,
                mRightSkillDmg,
                mRightSkillAR);

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
