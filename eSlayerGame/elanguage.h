#ifndef ELANGUAGE_H
#define ELANGUAGE_H

#include <string>
#include <vector>

struct eLanguage {
    eLanguage(const std::string& name,
              const std::string& suffix);

    std::string fName;
    std::string fSuffix;

    static eLanguage sLanguage;
    static std::vector<eLanguage> sLanguages;

    static void setLanguage(const std::string& name);

    static void load();
private:
    static bool sLoaded;
};

#endif // ELANGUAGE_H
