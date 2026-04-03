#ifndef ETILESTEXTURES_H
#define ETILESTEXTURES_H

#include "etiletextures.h"

#include <eSlayerHelpers/estringidmapvector.h>

class eTilesTextures {
public:
    eTilesTextures(const std::string& dirName);
protected:
    void loadImpl();
    int idImpl(const std::string& name);
    eTileTextures& getImpl(const std::string& name);
    eTileTextures& getImpl(const int id);
private:
    bool mLoaded = false;
    const std::string mDirName;
    eStringIdMapVector<eTileTextures> mTexs;
};

#endif // ETILESTEXTURES_H
