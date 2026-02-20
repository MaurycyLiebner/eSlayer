#ifndef ELANGUAGE_H
#define ELANGUAGE_H

#include <map>
#include <string>

class eLanguage {
public:
    eLanguage();

    static bool load();

    static const std::string& text(const int g, const int s);
private:
    static eLanguage sInstance;
    const std::string& textImpl(const int g, const int s);

    bool loadImpl();

    bool mLoaded = false;
    using eGroup = std::map<int, std::string>;
    using eStrings = std::map<int, eGroup>;
    eStrings mText;
};

#endif // ELANGUAGE_H
