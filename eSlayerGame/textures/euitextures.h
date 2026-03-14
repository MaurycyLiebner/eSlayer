#ifndef EUITEXTURES_H
#define EUITEXTURES_H

#include <map>
#include <string>
#include <memory>

class SDL_Renderer;
class eTexture;

class eUITextures {
public:        
    static void sLoad(SDL_Renderer * const r);

    static std::map<std::string, std::shared_ptr<eTexture>>
    sSkillIcons;
    static std::shared_ptr<eTexture>
    sWalkIcon;
    static std::shared_ptr<eTexture>
    sRunIcon;
private:
    static bool sLoaded;
};


#endif // EUITEXTURES_H
