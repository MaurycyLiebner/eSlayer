#ifndef ESOUNDSBASE_H
#define ESOUNDSBASE_H

#include "estringidmapvector.h"

class eSoundsBase {
public:
    static void load();

    static eStringIdMapVector<std::vector<std::string>> sSounds;
private:
    static bool sLoaded;
};

#endif // ESOUNDSBASE_H
