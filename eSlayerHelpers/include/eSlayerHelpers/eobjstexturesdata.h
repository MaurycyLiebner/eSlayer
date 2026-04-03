#ifndef EOBJSTEXTURESDATA_H
#define EOBJSTEXTURESDATA_H

#include "etiletexturesdata.h"

class ESLAYERHELPERS_API eObjsTexturesData : public eTileTexturesData {
public:
    eObjsTexturesData();

    static int id(const std::string& name);
    static eTileTextureData& get(const std::string& name);
    static eTileTextureData& get(const int id);
    static void load();
    using eIter = eTileTexturesData::eIter;
    static void forEach(const eIter& iter);
private:
    static eObjsTexturesData sInstance;
};

#endif // EOBJSTEXTURESDATA_H
