#include "equestswidget.h"

#include "../elabel.h"
#include "../../names/equesttext.h"
#include "../../textures/euitextures.h"
#include "../../etext.h"

#include <eSlayerHelpers/evectorhelpers.h>

eSlayerQuests eQuestsWidget::sState;
bool eQuestsWidget::sUpdated = false;

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
    bool mousePressEvent(const eMouseEvent& e) override {
        if(mPressAction) mPressAction();
        return true;
    }

    void paintEvent(ePainter& p) override {
        const auto& tex = texture();
        if(!tex) return;
        p.drawTexture(0, 0, tex);
        const bool hovered = eWidget::hovered();
        if(hovered) {
            tex->setBlendMode(SDL_BLENDMODE_ADD);
            tex->setAlpha(128);
            p.drawTexture(0, 0, tex);
            tex->setBlendMode(SDL_BLENDMODE_BLEND);
            tex->clearAlphaMod();
        }
    }
private:
    eAction mPressAction;
};

struct eExtQuest {
    eExtQuest(const uint8_t id,
              const std::string& name,
              const eQuest& quest) :
        fId(id), fName(name),
        fQuest(quest) {}
    uint8_t fId;
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
    const auto updated = getUpdated(newState);
    for(const auto& it : eQuests::sQuests) {
        const uint8_t id = it.fId;
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

        const int width = 400*mult;

        const auto titleLabel = new eLabel(window());
        titleLabel->resize(width, 20*mult);

        const auto textLabel = new eLabel(window());
        textLabel->resize(width, 250*mult);
        textLabel->setWrapWidth(width);

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
        bool lockCurrent = false;
        for(const auto& q : qa.fQuests) {
            const auto qw = new eQuestWidget(window());
            const uint8_t id = q.fId;
            const auto stage = sState.stage(id);
            eAction pressAction;
            if(stage > 0) {
                pressAction = [q, setCurrent]() {
                    setCurrent(q);
                };
                if(!lockCurrent) {
                    lockCurrent = eVectorHelpers::contains(updated, id);
                    current = q;
                }
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

    sUpdated = false;
}

bool eQuestsWidget::checkUpdated(
    const eSlayerQuests& newState) {
    if(sUpdated) return true;
    const auto vec = getUpdated(newState);
    sUpdated = !vec.empty();
    return sUpdated;
}

bool eQuestsWidget::updated() {
    return sUpdated;
}

std::vector<uint8_t>
eQuestsWidget::getUpdated(
    const eSlayerQuests& newState) {
    std::vector<uint8_t> updated;
    for(const auto& it : eQuests::sQuests) {
        const uint8_t id = it.fId;
        const auto oldStage = sState.stage(id);
        const auto newStage = newState.stage(id);
        if(newStage > oldStage) {
            updated.emplace_back(id);
        }
    }
    return updated;
}
