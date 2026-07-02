#include "equestswidget.h"

#include "../elabel.h"
#include "../../names/equesttext.h"
#include "../../textures/euitextures.h"
#include "../../etext.h"

eSlayerQuests eQuestsWidget::sState;

class eQuestWidget : public eLabel {
public:
    using eLabel::eLabel;

    void initialize(const uint8_t questId,
                    const uint8_t stageId,
                    const eAction& pressAction) {
        setNoPadding();
        mPressAction = pressAction;
        const auto bname = eQuests::sQuests.name(questId);
        const auto& quest = eQuests::sQuests.get(questId);
        const auto texId = eUITextures::sQuestIcons.id(bname);
        const auto& icon = eUITextures::sQuestIcons.get(texId);
        const auto nstages = quest.nStages();
        const bool finished = stageId >= nstages;
        const bool notStarted = stageId == 0;
        if(notStarted) {
            setTexture(icon.fNotStarted);
        } else if(finished) {
            setTexture(icon.fFinished);
        } else {
            setTexture(icon.fStarted);
        }
        fitContent();
    }
protected:
    bool mousePressEvent(const eMouseEvent& e) {
        if(mPressAction) mPressAction();
        return true;
    }
private:
    eAction mPressAction;
};

struct eExtQuest {
    eExtQuest(const int id,
              const std::string& name,
              const eQuest& quest) :
        fId(id), fName(name),
        fQuest(quest) {}
    int fId;
    std::string fName;
    eQuest fQuest;
};

struct eQuestAct {
    int fActId;
    std::vector<eExtQuest> fQuests;
};

void eQuestsWidget::initialize(
    const uint8_t cActId,
    const eSlayerQuests& newState) {
    const auto& res = resolution();
    const float mult = res.multiplier();
    const int p = res.largePadding();

    uint8_t maxAct = 0;
    std::map<uint8_t, eQuestAct> qacts;
    std::vector<uint8_t> updated;
    for(const auto& it : eQuests::sQuests) {
        const uint8_t id = it.fId;
        const auto oldStage = sState.stage(id);
        const auto newStage = newState.stage(id);
        if(newStage > oldStage) {
            updated.emplace_back(id);
        }

        const auto& q = it.fValue;
        auto& a = qacts[q.fAct];
        a.fActId = q.fAct;
        a.fQuests.emplace_back(id, it.fName, q);
        maxAct = std::max(maxAct, q.fAct);
    }
    sState = newState;

    std::vector<eAct> acts;
    eAct currentAct;
    for(const auto& it : qacts) {
        const auto& qa = it.second;
        const auto w = new eWidget(window());
        w->setNoPadding();

        auto& a = acts.emplace_back();
        a.fActId = qa.fActId;
        a.fWidget = w;
        if(qa.fActId == cActId) {
            currentAct = a;
        }

        const auto iconsWidget = new eWidget(window());
        iconsWidget->setNoPadding();

        const auto titleLabel = new eLabel(window());
        titleLabel->resize(400*mult, 20*mult);

        const auto textLabel = new eLabel(window());
        textLabel->resize(400*mult, 250*mult);

        const auto setCurrent =
            [this, titleLabel, textLabel](
            const eExtQuest& q) {
            const auto& title = eQuestText::title(q.fName);
            titleLabel->setText(title);
            titleLabel->fitContent();
            titleLabel->align(eAlignment::hcenter);

            const bool f = sState.finished(q.fId);
            if(f) {
                const auto& text = eText::text(21, 0);
                textLabel->setText(text);
            } else {
                const auto stage = sState.stage(q.fId);
                const auto& text = eQuestText::text(q.fName, stage);
                textLabel->setText(text);
            }
            textLabel->fitContent();
        };

        std::optional<eExtQuest> current;
        for(const auto& q : qa.fQuests) {
            const auto qw = new eQuestWidget(window());
            const int id = q.fId;
            const auto stage = sState.stage(id);
            eAction pressAction;
            if(stage > 0) {
                pressAction = [q, setCurrent]() {
                    setCurrent(q);
                };
                current = q;
            }
            qw->initialize(id, stage, pressAction);
            iconsWidget->addWidget(qw);
        }

        iconsWidget->stackHorizontally(p);
        iconsWidget->fitContent();

        w->addWidget(iconsWidget);
        w->addWidget(titleLabel);
        w->addWidget(textLabel);

        w->stackVertically(p);
        w->fitContent();

        if(current) setCurrent(*current);
    }

    eActsWidget::initialize("", acts, currentAct);
}
