#ifndef ESCREENBASE_H
#define ESCREENBASE_H

#include "../widgets/elabel.h"

class eScreenBase : public eLabel {
public:
    using eLabel::eLabel;
protected:
    eWidget* addInner();
};

#endif // ESCREENBASE_H
