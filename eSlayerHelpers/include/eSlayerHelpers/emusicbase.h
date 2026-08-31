#ifndef EMUSICBASE_H
#define EMUSICBASE_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"

class ESLAYERHELPERS_API eMusicBase {
public:
    static void load();

    static eStringIdMapVector<std::vector<std::string>> sMusic;
private:
    static bool sLoaded;
};

#endif // EMUSICBASE_H
