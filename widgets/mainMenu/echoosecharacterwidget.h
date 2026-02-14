#ifndef ECHOOSECHARACTERWIDGET_H
#define ECHOOSECHARACTERWIDGET_H

#include "../ewidget.h"

#include <string>

class eCharacters;

class eChooseCharacterWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const eCharacters& chars);

    const std::string& current() const { return mCurrent; }
private:
    std::string mCurrent;
};

#endif // ECHOOSECHARACTERWIDGET_H
