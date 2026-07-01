#ifndef ETALKTEXT_H
#define ETALKTEXT_H

#include <map>
#include <string>

class eTalkText {
public:
    static const std::string& title(
        const std::string& bname);
    static const std::string& text(
        const std::string& bname);

    static bool load();
    static bool reload();
private:
    static eTalkText sInstance;

    bool mLoaded = false;
    std::map<std::string, std::string> mValues;
};

#endif // ETALKTEXT_H
