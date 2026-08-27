#ifndef EGAMEDIR_H
#define EGAMEDIR_H

#include "eslayerhelpersexport.h"

#include <string>

class ESLAYERHELPERS_API eGameDir {
public:
    static std::string path(const std::string& path);
    static std::string windowSettingsPath();
    static std::string renderSettingsPath();
    static std::string soundSettingsPath();

    static std::string sExePath;
};

#endif // EGAMEDIR_H
