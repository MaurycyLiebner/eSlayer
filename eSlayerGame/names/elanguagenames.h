#ifndef ELANGUAGENAMES_H
#define ELANGUAGENAMES_H

#include <map>
#include <string>

class eLanguageNames {
public:
    static const std::string& name(const std::string& bname);
    static bool load();
    static bool reload();
private:
    static eLanguageNames sInstance;

    bool mLoaded = false;
    std::map<std::string, std::string> mNames;
};

#endif // ELANGUAGENAMES_H
