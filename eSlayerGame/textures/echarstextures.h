#ifndef ECHARSTEXTURES_H
#define ECHARSTEXTURES_H

#include "echartextures.h"

class eCharsTextures {
public:
    static eCharTextures& get(const std::string& name);
    static eCharTextures& get(const int id);
    static void load();
    static void clear(const bool forButton);
private:
    void clearImpl(const bool forButton);
    void loadImpl();
    eCharTextures& getImpl(const std::string& name);
    eCharTextures& getImpl(const int id);

    static eCharsTextures sInstance;

    bool mLoaded = false;
    eStringIdMapVector<eCharTextures> mChars;
};

#endif // ECHARSTEXTURES_H
