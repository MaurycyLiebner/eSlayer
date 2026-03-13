#ifndef EUITEXTURES_H
#define EUITEXTURES_H

class SDL_Renderer;

class eUITextures {
public:        
    static void sLoad(SDL_Renderer * const r);
private:
    static bool sLoaded;
};


#endif // EUITEXTURES_H
