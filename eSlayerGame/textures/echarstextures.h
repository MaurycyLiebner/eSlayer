#ifndef ECHARSTEXTURES_H
#define ECHARSTEXTURES_H

#include "echartextures.h"

class eCharsTextures {
public:
    eCharsTextures();

    static eCharTextures* get(const std::string& name);
    static void load();
private:
    void loadImpl();
    eCharTextures* getImpl(const std::string& name);

    static eCharsTextures sInstance;

    bool mLoaded = false;
    std::map<std::string, eCharTextures> mChars;
};

#endif // ECHARSTEXTURES_H
