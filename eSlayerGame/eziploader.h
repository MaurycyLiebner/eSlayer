#ifndef EZIPLOADER_H
#define EZIPLOADER_H

#include <zip.h>

#include <vector>
#include <string>

class eZipLoader {
public:
    eZipLoader();
    ~eZipLoader();

    bool opened() const { return mArchive; }
    void open(const std::string& path);
    bool close();

    std::vector<std::byte> load(const std::string& path);
private:
    zip_t* mArchive = nullptr;
};

#endif // EZIPLOADER_H
