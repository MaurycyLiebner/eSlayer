#ifndef EDUNGEON_H
#define EDUNGEON_H

#include "eSlayerMapGenerator/emapsettings.h"

#include <eSlayerHelpers/erect.h>

#include <memory>

class eMap;
struct eMapSettings;

struct eConnection {
    int fX;
    int fY;
    int fW;
    int fH;
};

class eDungeon {
public:
    eDungeon();
    eDungeon(const std::string& name, const int x, const int y, const int w, const int h,
             const std::shared_ptr<eMap>& map, const eAreaSettings& settings,
             const eMapSettings& mapSettings, const int margin);

    void addConnection(const eConnection& conn);

    void shift(const int dx, const int dy);

    eRect rect() const;

    void setExtendedRect(const eRect& rect);

    void generate() const;

    void generateWalls() const;

    const std::string& name() const;
private:
    std::string mName;

    eRect mExtendedRect;

    int mX;
    int mY;
    int mWidth;
    int mHeight;

    int mMargin;

    std::vector<eConnection> mConnecitons;

    std::shared_ptr<eMap> mMap;
    eAreaSettings mSettings;
    eMapSettings mMapSettings;
};

#endif // EDUNGEON_H
