#ifndef EWAYPOINTWIDGET_H
#define EWAYPOINTWIDGET_H

#include "ebgwidget.h"

#include "eSlayerHelpers/eobject.h"

class eWaypointWidget : public eBgWidget {
public:
    using eBgWidget::eBgWidget;

    using eWaypointAction = std::function<
        void(const eAreaIds& area)>;

    void initialize(const uint8_t cActId,
                    const eAreaIds& area,
                    const eWaypointAction& waction);
};

#endif // EWAYPOINTWIDGET_H
