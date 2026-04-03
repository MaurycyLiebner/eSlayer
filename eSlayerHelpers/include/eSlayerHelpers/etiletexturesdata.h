#ifndef ETILETEXTURESDATA_H
#define ETILETEXTURESDATA_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"

struct eTileTextureData {
    std::string fDirName;
    std::string fName;
};

class ESLAYERHELPERS_API eTileTexturesData {
public:
    eTileTexturesData(const std::string& dirName);
    using eIt = eStringIdMapVector<eTileTextureData>::eIterateType<const eTileTextureData&>;
    using eIter = std::function<void(const eIt& data)>;
protected:
    void loadImpl();
    int idImpl(const std::string& name);
    eTileTextureData& getImpl(const std::string& name);
    eTileTextureData& getImpl(const int id);
    void forEachImpl(const eIter& iter) const;
private:
    bool mLoaded = false;
    const std::string mDirName;
    eStringIdMapVector<eTileTextureData> mTexs;
};

#endif // ETILETEXTURESDATA_H
