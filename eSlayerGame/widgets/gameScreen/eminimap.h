#ifndef EMINIMAP_H
#define EMINIMAP_H

#include "../ewidget.h"
#include "eminimaparea.h"

#include <eSlayerHelpers/epoint.h>
#include <eSlayerMapGenerator/emap.h>

class eStatusWidget;
struct eGameSettings;

class eMiniMap : public eWidget {
public:
    eMiniMap(eMainWindow* const window);
    ~eMiniMap();

    static void clearAll();

    void initialize(const eGameSettings& settings);

    void setMap(const std::shared_ptr<eMap>& map);
    void setPos(const ePointF& pos);
    void setShowMap(const bool s);
    void switchShowMap();

    void setCharacterHorizontalPos(const float hpos)
    { mHPos = hpos; }

    static void sShowAreaName(const std::string& name);
protected:
    void paintEvent(ePainter& p) override;
private:
    static eMiniMap* sInstance;

    void showAreaName(const std::string& name);
    std::shared_ptr<eTexture>
    requestNameTex(const uint32_t clientId);

    std::shared_ptr<eMap> mMap;
    std::string mMapName;
    ePointF mPos;
    float mHPos = 0.5f;

    int fTileW = 0;
    int fTileH = 0;

    eStatusWidget* mStatusWidget = nullptr;

    bool mShowMap = false;
    static bool sShowMap;

    using eAreas = std::vector<std::vector<eMiniMapArea>>;
    eAreas mAreas;

    static std::map<std::string, eAreas> sAreasMap;

    int mAreaStrCounter = 0;
    std::shared_ptr<eTexture> mAreaStr;

    std::map<uint32_t, std::shared_ptr<eTexture>> mNameTexs;
};

#endif // EMINIMAP_H
