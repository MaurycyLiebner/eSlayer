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

    static bool checkUpdated(const eSlayerQuests& newState);
    static bool updated();
private:
    static std::vector<uint8_t> getUpdated(
        const eSlayerQuests& newState);

    static eSlayerQuests sState;
    static bool sUpdated;
};

#endif // EQUESTSWIDGET_H
