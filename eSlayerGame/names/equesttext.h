#ifndef EQUESTTEXT_H
#define EQUESTTEXT_H

#include <cstdint>
#include <map>
#include <string>

class eQuestText {
public:
    static const std::string& title(
        const std::string& bname);
    static const std::string& text(
        const std::string& bname,
        const uint8_t stage);

    static bool load();
    static bool reload();
private:
    static eQuestText sInstance;

    bool mLoaded = false;
    std::map<std::string, std::string> mValues;
};

#endif // EQUESTTEXT_H
