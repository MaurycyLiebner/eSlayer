#ifndef EFILELOADER_H
#define EFILELOADER_H

#include "eziploader.h"

#include <map>

#include <nlohmann/json.hpp>
using namespace nlohmann;

#include <rapidcsv.h>
using namespace rapidcsv;

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <SDL3_ttf/SDL_ttf.h>

class eTexture;

class eFileLoader {
public:
    eFileLoader(const bool useZip);

    static std::vector<std::byte> load(
        const std::string& dir,
        const std::string& path);
    static json parse(const std::string& dir,
                      const std::string& path);
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
                                 const std::string& path);
private:
    std::vector<std::byte> loadImpl(const std::string& dir,
                                    const std::string& path);
    json parseImpl(const std::string& dir,
                   const std::string& path);
    Document readCsvImpl(const std::string& dir,
                         const std::string& path);
    std::shared_ptr<eTexture> readTextureImpl(
        SDL_Renderer * const r,
        const std::string& dir,
        const std::string& path,
        const SDL_Color& colorKey = {0, 0, 0, 0});
    MIX_Audio* loadAudioImpl(MIX_Mixer * const mixer,
                             const std::string& dir,
                             const std::string& path);
    TTF_Font* loadTTFFontImpl(const int size,
                              const std::string& dir,
                              const std::string& path);

    static eFileLoader* sInstance;
    const bool mUseZip;
    std::map<std::string, eZipLoader> mZipLoaders;
};

#endif // EFILELOADER_H
