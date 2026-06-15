#include "ewaypointwidget.h"

#include "../elabel.h"
#include "../ebuttonbase.h"
#include "../../etext.h"
#include "../../names/eareanames.h"
#include "../mainMenu/emainmenubutton.h"

#include <eSlayerHelpers/ewaypoints.h>
#include <eSlayerMapGenerator/emapsettings.h>

class eWaypointLine : public eButtonBase {
public:
    using eButtonBase::eButtonBase;

    void initialize(const std::string& name,
                    const eAction& action,
                    const bool current) {
        setNoPadding();
        setText(name);
        // setTextAlignment(eAlignment::left);
        if(action) {
            setFontColor(eFontColor::white);
            setPressAction(action);
            setMouseEnterAction([&]() {
                setFontColor(eFontColor::blue);
            });
            setMouseLeaveAction([&]() {
                setFontColor(eFontColor::white);
            });
        } else {
            if(current) {
                setFontColor(eFontColor::blue);
            } else {
                setFontColor(eFontColor::gray);
            }
        }
    }
};

void eWaypointWidget::initialize(
    const uint8_t cActId,
    const uint8_t cMapId,
    const uint8_t cAreaId,
    const eWaypointAction& waction) {
    const auto innerW = new eWidget(window());
    innerW->setNoPadding();

    const auto title = eText::text(17, 0);
    const auto titleLabel = new eLabel(window());
    titleLabel->setNoPadding();
    titleLabel->setSmallFontSize();
    titleLabel->setText(title);
    titleLabel->fitContent();
    innerW->addWidget(titleLabel);

    const auto& res = resolution();
    const float mult = res.multiplier();
    const int p = res.largePadding();

    uint8_t maxAct = 0;
    for(const auto& w : eWaypoint::sWaypoints) {
        maxAct = std::max(maxAct, w.fActId);
    }

    const auto actsW = new eWidget(window());
    actsW->setNoPadding();

    const auto wW = new eWidget(window());
    wW->setNoPadding();

    const int singleW = 400*mult;
    const int singleH = 50*mult;

    wW->resize(singleW, 8*singleH);

    const auto toRoman = [](const uint8_t n) {
        if(n == 0) return "I";
        if(n == 1) return "II";
        if(n == 2) return "III";
        if(n == 3) return "IV";
        if(n == 4) return "V";
        if(n == 5) return "VI";
        if(n == 6) return "VII";
        if(n == 7) return "VIII";
        if(n == 8) return "IX";
        return "X";
    };

    struct eAct {
        uint8_t fActId;
        eMainMenuButton* fButton = nullptr;
        eWidget* fWidget = nullptr;

        bool operator==(const eAct& act) const {
            return fActId == act.fActId;
        }
    };

    const auto acts = std::make_shared<std::vector<eAct>>();

    const auto setCurrent = [acts](const eAct& act) {
        for(const auto& a : *acts) {
            const auto b = act.fButton;
            const auto w = act.fWidget;
            if(a == act) {
                w->show();
                b->setFontColor(eFontColor::white);
            } else {
                w->hide();
                b->setFontColor(eFontColor::gray);
            }
        }
    };

    eAct currentAct;
    for(uint8_t actId = 0; actId <= maxAct; actId++) {
        const auto r = toRoman(actId);
        const auto b = new eMainMenuButton(r, window());
        b->resize(singleW, singleH);
        actsW->addWidget(b);

        const auto w = new eWidget(window());
        w->setNoPadding();
        w->resize(wW->width(), wW->height());
        wW->addWidget(w);

        eAct act;
        act.fActId = actId;
        act.fWidget = w;
        act.fButton = b;
        if(cActId == actId) {
            currentAct = act;
        }
        b->setPressAction([act, setCurrent]() {
            setCurrent(act);
        });
        acts->emplace_back(act);
        for(const auto& way : eWaypoint::sWaypoints) {
            if(way.fActId != actId) continue;
            const auto& mapInfo = eMapsSettings::sMaps.get(way.fMapId);
            const auto nameBase = mapInfo.fAreas.name(way.fAreaId);
            const auto name = eAreaNames::name(nameBase);

            const auto line = new eWaypointLine(window());
            const bool current = way.fMapId == cMapId &&
                                 way.fAreaId == cAreaId;
            eAction action;
            if(!current/* && way.fKnown*/) {
                action = [way, waction]() {
                    waction(way.fMapId, way.fAreaId);
                };
            }
            line->resize(singleW, singleH);
            line->initialize(name, action, current);
            w->addWidget(line);
        }
        w->stackVertically();
    }
    setCurrent(currentAct);

    actsW->stackHorizontally(p);
    actsW->fitContent();
    innerW->addWidget(actsW);

    innerW->addWidget(wW);

    innerW->stackVertically(p);
    innerW->fitContent();

    setup(innerW);
    titleLabel->align(eAlignment::hcenter);
}
