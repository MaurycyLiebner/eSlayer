#include "eSlayerHelpers/eziploader.h"

#include "eSlayerHelpers/eexceptions.h"

#include <cstring>

eZipLoader::eZipLoader() {}

eZipLoader::~eZipLoader() {
    close();
}

void eZipLoader::open(const std::string& path) {
    int err = 0;
    mArchive = zip_open(path.c_str(), ZIP_RDONLY, &err);
    if(!mArchive) {
        const auto errStr = std::to_string(err);
        eRuntimeThrow("Failed to open zip archive " + errStr);
    }
}

bool eZipLoader::close() {
    if(!mArchive) return false;
    zip_close(mArchive);
    mArchive = nullptr;
    return true;
}

std::vector<std::byte> eZipLoader::load(const std::string& path) {
    if(!mArchive) {
        eRuntimeThrow("Zip archive not opened");
    }

    const auto index = zip_name_locate(mArchive, path.c_str(), 0);
    if(index < 0) {
        eRuntimeThrow("File not found in zip archive");
    }

    zip_stat_t sb;
    if(zip_stat_index(mArchive, index, 0, &sb) != 0) {
        eRuntimeThrow("Failed to stat file in zip");
    }

    const auto zf = zip_fopen_index(mArchive, index, 0);
    if(!zf) {
        eRuntimeThrow("Failed to open file in zip");
    }

    std::vector<std::byte> buffer(sb.size);

    const auto bytesRead = zip_fread(zf, buffer.data(), sb.size);
    if(bytesRead < 0 || static_cast<zip_uint64_t>(bytesRead) != sb.size) {
        zip_fclose(zf);
        eRuntimeThrow("Failed to read complete file");
    }

    zip_fclose(zf);

    return buffer;
}
