#ifndef EAREANAMES_H
#define EAREANAMES_H

#include <map>
#include <string>

class eAreaNames {
public:
    static const std::string&
    name(const std::string& nameBase);
    static bool load();
    static bool reload();
private:
    static eAreaNames sInstance;

    bool mLoaded = false;
    std::map<std::string, std::string> mNames;
};

#endif // EAREANAMES_H
