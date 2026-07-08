#ifndef EWAYPOINTLINE_H
#define EWAYPOINTLINE_H

#include "../ebuttonbase.h"

class eWaypointLine : public eButtonBase {
public:
    using eButtonBase::eButtonBase;

    void initialize(const std::string& name,
                    const eAction& action,
                    const bool current);
};

#endif // EWAYPOINTLINE_H
