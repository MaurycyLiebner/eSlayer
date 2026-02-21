#include "efileloader.h"

#include "egamedir.h"
#include "textures/etexture.h"

#include <filesystem>
#include <fstream>

eFileLoader* eFileLoader::sInstance = nullptr;

eFileLoader::eFileLoader(const bool useZip) :
    mUseZip(useZip) {
    if(sInstance) {
        printf("Duplicate file loader created.\n");
    }
    sInstance = this;
}

std::vector<std::byte> eFileLoader::load(
    const std::string& dir,
    const std::string& path) {
    if(!sInstance) {
        throw std::runtime_error("No available file loader.");
    }
    return sInstance->loadImpl(dir, path);
}

json eFileLoader::parse(const std::string& dir,
                        const std::string& path) {
    if(!sInstance) {
        throw std::runtime_error("No available file loader.");
    }
    return sInstance->parseImpl(dir, path);
}

Document eFileLoader::readCsv(const std::string& dir,
                              const std::string& path) {
    if(!sInstance) {
        throw std::runtime_error("No available file loader.");
    }
    return sInstance->readCsvImpl(dir, path);
}

std::shared_ptr<eTexture> eFileLoader::readTexture(
    SDL_Renderer * const r,
    const std::string& dir,
    const std::string& path,
    const SDL_Color& colorKey) {
    if(!sInstance) {
        throw std::runtime_error("No available file loader.");
    }
    return sInstance->readTextureImpl(r, dir, path, colorKey);
}

MIX_Audio* eFileLoader::loadAudio(MIX_Mixer * const mixer,
                                  const std::string& dir,
                                  const std::string& path) {
    if(!sInstance) {
        throw std::runtime_error("No available file loader.");
    }
    return sInstance->loadAudioImpl(mixer, dir, path);
}

TTF_Font* eFileLoader::loadTTFFont(const int size,
                                   const std::string& dir,
                                   const std::string& path) {
    if(!sInstance) {
        throw std::runtime_error("No available file loader.");
    }
    return sInstance->loadTTFFontImpl(size, dir, path);
}

std::string gFilePath(const std::string& dir,
                      const std::string& path) {
    return eGameDir::path(dir + "/" + path);
}

std::vector<std::byte> eFileLoader::loadImpl(
    const std::string& dir,
    const std::string& path) {
    if(mUseZip) {
        auto& zip = mZipLoaders[dir];
        if(!zip.opened()) {
            zip.open(eGameDir::path(dir + ".zip"));
        }
        return zip.load(path);
    } else {
        const auto filePath = gFilePath(dir, path);
        std::ifstream f(filePath);
        const auto length { std::filesystem::file_size(path) };
        std::vector<std::byte> data(length);
        f.read(reinterpret_cast<char*>(data.data()), static_cast<long>(length));
        return data;
    }
}

json eFileLoader::parseImpl(const std::string& dir,
                            const std::string& path) {
    if(mUseZip) {
        const auto data = load(dir, path);
        return json::parse(data.begin(), data.end());
    } else {
        const auto filePath = gFilePath(dir, path);
        std::ifstream f(filePath);
        return json::parse(f);
    }
}

Document eFileLoader::readCsvImpl(const std::string& dir,
                                  const std::string& path) {
    if(mUseZip) {
        const auto data = load(dir, path);
        std::string csvText(reinterpret_cast<const char*>(data.data()), data.size());
        std::istringstream stream(csvText);
        return Document(stream, rapidcsv::LabelParams(-1, -1));
    } else {
        return Document(gFilePath(dir, path),
                        rapidcsv::LabelParams(-1, -1));
    }
}

std::shared_ptr<eTexture> eFileLoader::readTextureImpl(
    SDL_Renderer* const r,
    const std::string& dir,
    const std::string& path,
    const SDL_Color& colorKey) {
    const auto tex = std::make_shared<eTexture>();
    if(mUseZip) {
        const auto data = load(dir, path);
        const auto io = SDL_IOFromConstMem(
            reinterpret_cast<const void*>(data.data()),
            data.size()
            );

        if(io) {
            const auto surf = IMG_Load_IO(io, true);
            tex->load(r, surf, colorKey);
        }
    } else {
        tex->load(r, gFilePath(dir, path), colorKey);
    }
    if(!tex->tex()) {
        printf("Failed to load texture '%s'!\n SDL Error: %s\n",
               path.c_str(), SDL_GetError());
    }
    return tex;
}

MIX_Audio* eFileLoader::loadAudioImpl(
    MIX_Mixer * const mixer,
    const std::string& dir,
    const std::string& path) {
    MIX_Audio* audio = nullptr;
    if(mUseZip) {
        const auto data = load(dir, path);
        const auto io = SDL_IOFromConstMem(
            reinterpret_cast<const void*>(data.data()),
            data.size()
            );

        if(!io) return nullptr;
        audio = MIX_LoadAudio_IO(mixer, io, false, true);
    } else {
        const auto filePath = gFilePath(dir, path);
        audio = MIX_LoadAudio(mixer, filePath.c_str(), false);
    }
    if(!audio) {
        printf("Failed to load audio '%s'!\n SDL_mixer Error: %s\n",
               path.c_str(), SDL_GetError());
    }
    return audio;
}

TTF_Font* eFileLoader::loadTTFFontImpl(const int size,
                                       const std::string& dir,
                                       const std::string& path) {
    TTF_Font* result = nullptr;
    if(mUseZip) {
        const auto data = load(dir, path);
        const auto io = SDL_IOFromConstMem(
            reinterpret_cast<const void*>(data.data()),
            data.size()
            );
        result = TTF_OpenFontIO(io, true, size);
    } else {
        const auto filePath = gFilePath(dir, path);
        result = TTF_OpenFont(filePath.c_str(), size);
    }
    if(!result) {
        printf("Failed to load font! "
               "SDL_ttf Error: %s\n",
               SDL_GetError());
    }
    return result;
}
