#ifndef ETILESTEXTURES_H
#define ETILESTEXTURES_H

#include "etiletextures.h"

class eTilesTextures {
public:
    eTilesTextures(const std::string& dirName);
protected:
    void loadImpl();
    eTileTextures* getImpl(const std::string& name);
private:
    bool mLoaded = false;
    const std::string mDirName;
    std::map<std::string, eTileTextures> mTexs;
};

#endif // ETILESTEXTURES_H
