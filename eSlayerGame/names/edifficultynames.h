#ifndef EDIFFICULTYNAMES_H
#define EDIFFICULTYNAMES_H

#include <map>
#include <string>

class eDifficultyNames {
public:
    static std::string name(const int difficultyId);
    static bool load();
    static bool reload();
private:
    static eDifficultyNames sInstance;

    bool mLoaded = false;
    std::map<int, std::string> mNames;
};

#endif // EDIFFICULTYNAMES_H
