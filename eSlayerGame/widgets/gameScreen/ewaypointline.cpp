#include "ewaypointline.h"

void eWaypointLine::initialize(
    const std::string& name,
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
