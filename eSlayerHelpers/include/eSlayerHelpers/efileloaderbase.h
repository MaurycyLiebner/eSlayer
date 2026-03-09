#ifndef EFILELOADERBASE_H
#define EFILELOADERBASE_H

#include "eslayerhelpersexport.h"

#include "eziploader.h"

#include <map>

#include <nlohmann/json.hpp>
using namespace nlohmann;

class ESLAYERHELPERS_API eFileLoaderBase {
public:
    static ordered_json parse(const std::string& dir,
                              const std::string& path);

    static std::string sFilePath(const std::string& dir,
                                 const std::string& path);

    static std::vector<std::byte> load(const std::string& dir,
                                       const std::string& path);

    static std::map<std::string, eZipLoader> sZipLoaders;

    static eFileLoaderBase sInstance;
};

#endif // EFILELOADERBASE_H
