#ifndef EMUSICBASE_H
#define EMUSICBASE_H

#include "estringidmapvector.h"

class eMusicBase {
public:
    static void load();

    static eStringIdMapVector<std::vector<std::string>> sMusic;
private:
    static bool sLoaded;
};

#endif // EMUSICBASE_H
