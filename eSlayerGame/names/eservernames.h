#ifndef ESERVERNAMES_H
#define ESERVERNAMES_H

#include <map>
#include <string>

class eServerNames {
public:
    static std::string name(const std::string& key);
    static void load();
private:
    static std::map<std::string, std::string> sMap;
};

#endif // ESERVERNAMES_H
