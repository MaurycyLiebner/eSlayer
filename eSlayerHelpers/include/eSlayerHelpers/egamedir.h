#ifndef EGAMEDIR_H
#define EGAMEDIR_H

#include <string>

class eGameDir {
public:
    static std::string path(const std::string& path);
    static std::string windowSettingsPath();
};

#endif // EGAMEDIR_H
