#ifndef EWAYPOINTWIDGET_H
#define EWAYPOINTWIDGET_H

#include "eactswidget.h"

#include <eSlayerHelpers/eareaids.h>

struct eWaypoints;

class eWaypointWidget : public eActsWidget {
public:
    using eActsWidget::eActsWidget;

    using eWaypointAction = std::function<
        void(const eAreaIds& area)>;

    void initialize(const uint8_t cActId,
                    const eAreaIds& area,
                    const eWaypointAction& waction,
                    const eWaypoints& waypoints);
};

#endif // EWAYPOINTWIDGET_H
