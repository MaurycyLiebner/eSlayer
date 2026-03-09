#ifndef EFILELOADER_H
#define EFILELOADER_H

#include <eSlayerHelpers/efileloaderbase.h>

#include <rapidcsv.h>
using namespace rapidcsv;

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <SDL3_ttf/SDL_ttf.h>

class eTexture;

class eFileLoader : public eFileLoaderBase {
public:
    static Document readCsv(const std::string& dir,
                            const std::string& path);
    static std::shared_ptr<eTexture> readTexture(
        SDL_Renderer * const r,
        const std::string& dir,
        const std::string& path,
        const SDL_Color& colorKey = {0, 0, 0, 0});
    static MIX_Audio* loadAudio(MIX_Mixer * const mixer,
                                const std::string& dir,
                                const std::string& path);
    static TTF_Font* loadTTFFont(const int size,
                                 const std::string& dir,
                                 const std::string& path);};

#endif // EFILELOADER_H
