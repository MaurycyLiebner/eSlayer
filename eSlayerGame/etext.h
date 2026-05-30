#ifndef ETEXT_H
#define ETEXT_H

#include <map>
#include <string>
#include <vector>

class eText {
public:
    eText();

    static bool load();
    using eGroup = std::map<int, std::string>;
    using eStrings = std::map<int, eGroup>;
    static bool parse(const std::string& path,
                      eStrings& strings);
    static bool parse(const std::vector<std::byte>& data,
                      eStrings& strings);

    static const std::string& text(const int g, const int s);
private:
    static eText sInstance;
    const std::string& textImpl(const int g, const int s);

    bool loadImpl();

    bool mLoaded = false;
    eStrings mText;
};

#endif // ETEXT_H
