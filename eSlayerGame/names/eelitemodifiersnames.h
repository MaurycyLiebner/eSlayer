#ifndef EELITEMODIFIERSNAMES_H
#define EELITEMODIFIERSNAMES_H

#include <map>
#include <string>

class eEliteModifiersNames {
public:
    static std::string name(const int eliteId);
    static bool load();
private:
    static eEliteModifiersNames sInstance;

    bool mLoaded = false;
    std::map<int, std::string> mNames;
};

#endif // EELITEMODIFIERSNAMES_H
