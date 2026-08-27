#ifndef EMISSILESINFO_H
#define EMISSILESINFO_H

#include "eslayerhelpersexport.h"

#include <eSlayerHelpers/estringidmapvector.h>

struct eMissileAnim {
    int fNDirs = 0;
    int fNFrames = 0;
    std::string fPath;
};

enum class eMissileType {
    regular, explosion, overlay,
    curse, aura
};

class ESLAYERHELPERS_API eMissileInfo {
    friend class eMissilesInfo;
    friend class eMissileTextures;
public:
    int nFrames(const int animId) const;
    int nDirs(const int animId) const;

    int animId(const std::string& name) const;

    int appearAnimId() const { return mAppearAnimId; }
    int baseAnimId() const { return mBaseAnimId; }
    int disappearAnimId() const { return mDisappearAnimId; }
    int hitAnimId() const { return mHitAnimId; }
    int stayAnimId() const { return mStayAnimId; }

    int appearSoundId() const { return mAppearSoundId; }
    int hitSoundId() const { return mHitSoundId; }
    int loopSoundId() const { return mLoopSoundId; }
    int disappearSoundId() const { return mDisappearSoundId; }

    float lighting() const { return mLighting; }
    float radius() const { return mRadius; }

    eMissileType type() const { return mType; }

    const eMissileAnim& anim(const int id) const;
private:
    eMissileType mType = eMissileType::regular;

    int mAppearAnimId = -1;
    int mBaseAnimId = -1;
    int mDisappearAnimId = -1;
    int mHitAnimId = -1;
    int mStayAnimId = -1;

    int mAppearSoundId = -1;
    int mHitSoundId = -1;
    int mLoopSoundId = -1;
    int mDisappearSoundId = -1;

    float mLighting = 0.f;
    float mRadius = 0.f;
    eStringIdMapVector<eMissileAnim> mAnims;
};

class ESLAYERHELPERS_API eMissilesInfo {
public:
    static void load();

    static eStringIdMapVector<eMissileInfo>
    sMissiles;

    static int sFleshId;
    static int sIceId;
private:
    static bool sLoaded;
};

#endif // EMISSILESINFO_H
