#include "efileloader.h"

#include "textures/etexture.h"
#include "eloadtexthelper.h"
#include "etext.h"
#include "elanguage.h"

#include <eSlayerHelpers/eexceptions.h>
#include <eSlayerHelpers/erunsettings.h>

Document eFileLoader::readCsv(const std::string& dir,
                              const std::string& path) {
    if(eRunSettings::sUseZip) {
        const auto data = sInstance.load(dir, path);
        std::string csvText(reinterpret_cast<const char*>(data.data()), data.size());
        std::istringstream stream(csvText);
        return Document(stream, rapidcsv::LabelParams(-1, -1));
    } else {
        return Document(sFilePath(dir, path),
                        rapidcsv::LabelParams(-1, -1));
    }
}

std::shared_ptr<eTexture> eFileLoader::readTexture(
    SDL_Renderer * const r,
    const std::string& dir,
    const std::string& path,
    const SDL_Color& colorKey) {
    const auto tex = std::make_shared<eTexture>();
    if(eRunSettings::sUseZip) {
        const auto data = sInstance.load(dir, path);
        const auto io = SDL_IOFromConstMem(
            reinterpret_cast<const void*>(data.data()),
            data.size()
            );

        if(io) {
            const auto surf = IMG_Load_IO(io, true);
            tex->load(r, surf, colorKey);
        }
    } else {
        tex->load(r, sFilePath(dir, path), colorKey);
    }
    if(!tex->tex()) {
        printf("Failed to load texture '%s'!\n SDL Error: %s\n",
               path.c_str(), SDL_GetError());
    }
    return tex;
}

MIX_Audio* eFileLoader::loadAudio(MIX_Mixer * const mixer,
                                  const std::string& dir,
                                  const std::string& path) {
    MIX_Audio* audio = nullptr;
    if(eRunSettings::sUseZip) {
        const auto data = sInstance.load(dir, path);
        const auto io = SDL_IOFromConstMem(
            reinterpret_cast<const void*>(data.data()),
            data.size()
            );

        if(!io) return nullptr;
        audio = MIX_LoadAudio_IO(mixer, io, false, true);
    } else {
        const auto filePath = sFilePath(dir, path);
        audio = MIX_LoadAudio(mixer, filePath.c_str(), false);
    }
    if(!audio) {
        eExceptions::logError(
            "Failed to load audio '" + path + "'!",
            SDL_GetError());
    }
    return audio;
}

TTF_Font* eFileLoader::loadTTFFont(const int size,
                                   const std::string& dir,
                                   const std::string& path) {
    TTF_Font* result = nullptr;
    if(eRunSettings::sUseZip) {
        const auto data = sInstance.load(dir, path);
        const auto io = SDL_IOFromConstMem(
            reinterpret_cast<const void*>(data.data()),
            data.size()
            );
        result = TTF_OpenFontIO(io, true, size);
    } else {
        const auto filePath = sFilePath(dir, path);
        result = TTF_OpenFont(filePath.c_str(), size);
    }
    if(!result) {
        eExceptions::logError(
            "Failed to load font '" + path + "'!",
            SDL_GetError());
    }
    return result;
}

std::map<std::string, std::string>
eFileLoader::loadNames(const std::string& dir,
                       std::string path) {
    const auto suffix = eLanguage::sLanguage.fSuffix;
    path = path + "_" + suffix + ".txt";
    std::map<std::string, std::string> result;
    if(eRunSettings::sUseZip) {
        const auto data = sInstance.load(dir, path);
        eLoadTextHelper::load(data, result);
    } else {
        const auto filePath = sFilePath(dir, path);
        eLoadTextHelper::load(filePath, result);
    }
    return result;
}

eFileLoader::eStrings
eFileLoader::loadText(
    const std::string& dir,
    std::string path) {
    const auto suffix = eLanguage::sLanguage.fSuffix;
    path = path + "_" + suffix + ".xml";
    eStrings result;
    if(eRunSettings::sUseZip) {
        const auto data = sInstance.load(dir, path);
        eText::parse(data, result);
    } else {
        const auto filePath = sFilePath(dir, path);
        eText::parse(filePath, result);
    }
    return result;
}
