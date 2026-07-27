#ifndef EUNITINDICATOR_H
#define EUNITINDICATOR_H

#include "ehealthindicator.h"

#include <eSlayerHelpers/eunitdata.h>

class eUnit;

class eUnitIndicator : public eHealthIndicator {
public:
    using eHealthIndicator::eHealthIndicator;

    void initialize();

    void setUnit(const std::shared_ptr<eUnit>& u,
                 const std::string& name);
protected:
    void paintEvent(ePainter& p) override;
private:
    using eHealthIndicator::initialize;

    std::weak_ptr<eUnit> mUnit;
    eLabel* mModsLabel = nullptr;

    eWidget* mImmunitiesW = nullptr;
    std::map<eUnitData::eImmunity, eLabel*> mImmunityLabels;
};

#endif // EUNITINDICATOR_H
