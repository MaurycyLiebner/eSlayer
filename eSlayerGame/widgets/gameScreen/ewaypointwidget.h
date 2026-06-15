#ifndef EWAYPOINTWIDGET_H
#define EWAYPOINTWIDGET_H

#include "ebgwidget.h"

class eWaypointWidget : public eBgWidget {
public:
    using eBgWidget::eBgWidget;

    using eWaypointAction = std::function<
        void(const uint8_t mapId, const uint8_t areaId)>;

    void initialize(const uint8_t cActId,
                    const uint8_t cMapId,
                    const uint8_t cAreaId,
                    const eWaypointAction& waction);
};

#endif // EWAYPOINTWIDGET_H
