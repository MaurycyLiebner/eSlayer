#ifndef EQUESTSWIDGET_H
#define EQUESTSWIDGET_H

#include "eactswidget.h"

#include <eSlayerHelpers/equests.h>
#include <eSlayerHelpers/eslayerquests.h>

class eQuestsWidget : public eActsWidget {
public:
    using eActsWidget::eActsWidget;

    void initialize(
        const uint8_t cActId,
        const eSlayerQuests& newState);
private:
    static eSlayerQuests sState;
};

#endif // EQUESTSWIDGET_H
