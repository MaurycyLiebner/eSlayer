#ifndef EOBJSTEXTURES_H
#define EOBJSTEXTURES_H

#include "etilestextures.h"

class eObjsTextures : public eTilesTextures {
public:
    static int id(const std::string& name);
    static eTileTextures& get(const std::string& name);
    static eTileTextures& get(const int id);
    static void load();
private:
    static eStringIdMapVector<eTileTextures> sInstance;
};

#endif // EOBJSTEXTURES_H
