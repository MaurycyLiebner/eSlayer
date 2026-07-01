#include "ewaypointwidget.h"

#include "../elabel.h"
#include "../ebuttonbase.h"
#include "../../etext.h"
#include "../../names/eareanames.h"

#include <eSlayerHelpers/ewaypoints.h>
#include <eSlayerHelpers/emapsettings.h>
#include <eSlayerHelpers/estringhelpers.h>

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
    const eAreaIds& area,
    const eWaypointAction& waction) {
    const auto title = eText::text(17, 0);

    const auto& res = resolution();
    const float mult = res.multiplier();

    const int singleW = 400*mult;
    const int singleH = 50*mult;

    std::vector<eAct> acts;

    uint8_t maxAct = 0;
    for(const auto& w : eWaypoint::sWaypoints) {
        maxAct = std::max(maxAct, w.fActId);
    }

    eAct currentAct;
    for(uint8_t actId = 0; actId <= maxAct; actId++) {
        const auto w = new eWidget(window());
        w->setNoPadding();
        w->resize(singleW, 8*singleH);

        auto& act = acts.emplace_back();
        act.fActId = actId;
        act.fWidget = w;
        if(cActId == actId) {
            currentAct = act;
        }
        for(const auto& way : eWaypoint::sWaypoints) {
            if(way.fActId != actId) continue;
            const auto& warea = way.fArea;
            const auto& mapInfo = eMapsSettings::sMaps.get(warea.fMapId);
            const auto nameBase = mapInfo.fAreas.name(warea.fAreaId);
            const auto name = eAreaNames::name(nameBase);

            const auto line = new eWaypointLine(window());
            const bool current = warea == area;
            eAction action;
            if(!current && way.fKnown) {
                action = [warea, waction]() {
                    waction(warea);
                };
            }
            line->resize(singleW, singleH);
            line->initialize(name, action, current);
            w->addWidget(line);
        }
        w->stackVertically();
    }

    eActsWidget::initialize(title, acts, currentAct);
}
