#ifndef ECHARSTEXTURES_H
#define ECHARSTEXTURES_H

#include "echartextures.h"

class eCharsTextures {
public:
    eCharsTextures();

    static eCharTextures* get(const std::string& name);
    static eCharTextures* get(const int id);
    static void load();
private:
    void loadImpl();
    eCharTextures* getImpl(const std::string& name);
    eCharTextures* getImpl(const int id);

    static eCharsTextures sInstance;

    bool mLoaded = false;
    std::vector<eCharTextures> mChars;
    std::map<std::string, int> mCharIdMap;
};

#endif // ECHARSTEXTURES_H
