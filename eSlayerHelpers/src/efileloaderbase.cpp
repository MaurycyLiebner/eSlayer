#include "eSlayerHelpers/efileloaderbase.h"

#include "eSlayerHelpers/egamedir.h"
#include "eSlayerHelpers/erunsettings.h"
#include "eSlayerHelpers/eexceptions.h"

#include <filesystem>
#include <fstream>

std::map<std::string, eZipLoader>
    eFileLoaderBase::sZipLoaders;
eFileLoaderBase eFileLoaderBase::sInstance;

std::string eFileLoaderBase::sFilePath(const std::string& dir,
                                       const std::string& path) {
    return eGameDir::path(dir + "/" + path);
}

ordered_json eFileLoaderBase::parse(const std::string& dir,
                                    const std::string& path) {
    try {
        if(eRunSettings::sUseZip) {
            const auto data = sInstance.load(dir, path);
            return ordered_json::parse(data.begin(), data.end());
        } else {
            const auto filePath = sFilePath(dir, path);
            std::ifstream f(filePath);
            return ordered_json::parse(f);
        }
    } catch(...) {
        eRuntimeThrow("Failed to parse " + path + " from " + dir);
    }
}

std::vector<std::byte> eFileLoaderBase::load(const std::string& dir,
                                             const std::string& path) {
    if(eRunSettings::sUseZip) {
        auto& zip = sZipLoaders[dir];
        if(!zip.opened()) {
            zip.open(eGameDir::path(dir + ".zip"));
        }
        return zip.load(path);
    } else {
        const auto filePath = sFilePath(dir, path);
        std::ifstream f(filePath);
        const auto length { std::filesystem::file_size(filePath) };
        std::vector<std::byte> data(length);
        f.read(reinterpret_cast<char*>(data.data()), static_cast<long>(length));
        return data;
    }
}
