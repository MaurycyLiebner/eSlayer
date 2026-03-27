#ifndef ECHARDATAINFO_H
#define ECHARDATAINFO_H

#include "eSlayerHelpers/echardata.h"
#include "eSlayerHelpers/estringidmapvector.h"

class ESLAYERHELPERS_API eCharDataInfo {
public:
    static eCharData& get(const std::string& name);
    static eCharData& get(const int id);
    static void load();
private:
    void loadImpl();
    eCharData& getImpl(const std::string& name);
    eCharData& getImpl(const int id);

    static eCharDataInfo sInstance;

    bool mLoaded = false;
    eStringIdMapVector<eCharData> mChars;
};

#endif // ECHARDATAINFO_H
