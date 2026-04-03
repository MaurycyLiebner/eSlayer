#ifndef ETERRSTEXTURESDATA_H
#define ETERRSTEXTURESDATA_H

#include "etiletexturesdata.h"

class ESLAYERHELPERS_API eTerrsTexturesData : public eTileTexturesData {
public:
    eTerrsTexturesData();

    static int id(const std::string& name);
    static eTileTextureData& get(const std::string& name);
    static eTileTextureData& get(const int id);
    static void load();
    using eIter = eTileTexturesData::eIter;
    static void forEach(const eIter& iter);
private:
    static eTerrsTexturesData sInstance;
};

#endif // ETERRSTEXTURESDATA_H
