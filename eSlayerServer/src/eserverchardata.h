#ifndef ESERVERCHARDATA_H
#define ESERVERCHARDATA_H

#include <eSlayerHelpers/echardata.h>
#include <eSlayerHelpers/estringidmapvector.h>

class eServerCharData {
public:
    eServerCharData();

    static eCharData* get(const std::string& name);
    static eCharData* get(const int id);
    static void load();
private:
    void loadImpl();
    eCharData* getImpl(const std::string& name);
    eCharData* getImpl(const int id);
    std::string nameImpl(const int id);

    static eServerCharData sInstance;

    bool mLoaded = false;
    eStringIdMapVector<eCharData> mChars;
};

#endif // ESERVERCHARDATA_H
