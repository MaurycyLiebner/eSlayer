#ifndef ESERVERCHARDATA_H
#define ESERVERCHARDATA_H

#include <eSlayerHelpers/echardata.h>

class eServerCharData {
public:
    eServerCharData();

    static eCharData* get(const std::string& name);
    static void load();
private:
    void loadImpl();
    eCharData* getImpl(const std::string& name);

    static eServerCharData sInstance;

    bool mLoaded = false;
    std::map<std::string, eCharData> mChars;
};

#endif // ESERVERCHARDATA_H
