#include "eSlayerHelpers/eobstaclesmap.h"

const int eObstaclesMap::sTileSize = 5;

void eObstaclesMap::initialize(const eObstacleFiller& filler,
                               const int w, const int h) {
    const int tw = (w + sTileSize - 1)/sTileSize;
    const int th = (h + sTileSize - 1)/sTileSize;
    mWidth = tw;
    mHeight = th;
    mFiller = filler;
    mTiles = std::vector<std::vector<eObstacleTile>>(
        th, std::vector<eObstacleTile>(tw));
}

void eObstaclesMap::fillAll() {
    for(int x = 0 ; x < mWidth; x++) {
        for(int y = 0; y < mHeight; y++) {
            auto& t = mTiles[y][x];
            t.fInitialized = true;
            mFiller(t.fWalk, t.fMissile, x, y);
        }
    }
}

bool eObstaclesMap::inside(
    const int x, const int y) const {
    if(x < 0) return false;
    if(y < 0) return false;
    if(x >= mWidth) return false;
    if(y >= mHeight) return false;
    return true;
}

eObstacleTile* eObstaclesMap::tile(
    const int x, const int y) {
    const bool r = inside(x, y);
    if(!r) return nullptr;
    return &mTiles[y][x];
}

void eObstaclesMap::eraseTile(
    const int x, const int y) {
    const bool r = inside(x, y);
    if(!r) return;
    mTiles[y][x] = eObstacleTile();
}

bool eObstaclesMap::walkable(
    const ePointF& pos) {
    return check(pos, true);
}

bool eObstaclesMap::walkable(
    const ePointF& from, const ePointF& to) {
    return check(from, to, true);
}

bool eObstaclesMap::obstacle(
    const ePointF& pos) {
    return !check(pos, false);
}

bool eObstaclesMap::obstacle(
    const ePointF& from, const ePointF& to) {
    return !check(from, to, false);
}

bool eObstaclesMap::check(const ePointF& pos,
                          const bool choice) {
    const float tileSize = static_cast<float>(sTileSize);
    const int x = static_cast<int>(std::floor(pos.fX / tileSize));
    const int y = static_cast<int>(std::floor(pos.fY / tileSize));
    const auto t = tile(x, y);
    if(!t) return true;

    bool& ini = t->fInitialized;
    auto& walk = t->fWalk;
    auto& missile = t->fMissile;
    if(!ini) {
        ini = true;
        mFiller(walk, missile, x , y);
    }

    for(const auto& o : (choice ? walk : missile)) {
        const bool r = o.inside(pos);
        if(r) return false;
    }

    return true;
}

bool eObstaclesMap::check(
    const ePointF& from,
    const ePointF& to,
    const bool choice) {
    const float tileSize = static_cast<float>(sTileSize);

    // Convert world position to tile indices
    int x = static_cast<int>(std::floor(from.fX / tileSize));
    int y = static_cast<int>(std::floor(from.fY / tileSize));

    const int endX = static_cast<int>(std::floor(to.fX / tileSize));
    const int endY = static_cast<int>(std::floor(to.fY / tileSize));

    // Direction
    const float dx = to.fX - from.fX;
    const float dy = to.fY - from.fY;

    const int stepX = (dx > 0.f) ? 1 : (dx < 0.f ? -1 : 0);
    const int stepY = (dy > 0.f) ? 1 : (dy < 0.f ? -1 : 0);

    // Distance to cross one tile
    const float tDeltaX =
        (dx != 0.f) ? (tileSize / std::abs(dx))
                    : std::numeric_limits<float>::infinity();

    const float tDeltaY =
        (dy != 0.f) ? (tileSize / std::abs(dy))
                    : std::numeric_limits<float>::infinity();

    // First vertical boundary
    float nextBoundaryX;
    if(stepX > 0) {
        nextBoundaryX = (x + 1) * tileSize;
    } else {
        nextBoundaryX = x * tileSize;
    }

    // First horizontal boundary
    float nextBoundaryY;
    if(stepY > 0) {
        nextBoundaryY = (y + 1) * tileSize;
    } else {
        nextBoundaryY = y * tileSize;
    }

    float tMaxX =
        (dx != 0.f) ? ((nextBoundaryX - from.fX) / dx)
                    : std::numeric_limits<float>::infinity();

    float tMaxY =
        (dy != 0.f) ? ((nextBoundaryY - from.fY) / dy)
                    : std::numeric_limits<float>::infinity();

    while(true) {
        // Check current tile
        const auto t = tile(x, y);
        if(!t) return false;

        bool& ini = t->fInitialized;
        auto& walk = t->fWalk;
        auto& missile = t->fMissile;
        if(!ini) {
            ini = true;
            mFiller(walk, missile, x , y);
        }

        for(const auto& o : (choice ? walk : missile)) {
            const bool r = o.lineIntersects(from, to);
            if(r) return false;
        }

        // Reached destination tile
        if(x == endX && y == endY) {
            break;
        }

        // Advance to next tile
        if(tMaxX < tMaxY) {
            tMaxX += tDeltaX;
            x += stepX;
        } else {
            tMaxY += tDeltaY;
            y += stepY;
        }
    }

    return true;
}
