#ifndef EOBJSTEXTURES_H
#define EOBJSTEXTURES_H

#include "etilestextures.h"

class eObjsTextures : public eTilesTextures {
public:
    eObjsTextures();

    static eTileTextures* get(const std::string& name);
    static void load();
private:
    static eObjsTextures sInstance;
};

#endif // EOBJSTEXTURES_H
