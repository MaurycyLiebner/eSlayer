#include "../include/eSlayerHelpers/epathfinder.h"

#include <cassert>
#include <cstdlib>
#include <deque>

ePathFinderPath ePathFinder::findPath(
    ePathFinderMap& map,
    const ePointF& from,
    const ePointF& to,
    bool& found) {
    map.nextIter();
    ePathFinderPath result;
    const float dist = ePointF::distance(from, to);
    if(dist < 0.5f) {
        found = true;
        return result;
    }

    const auto fromTile = ePathFinderMap::posToTile(from);
    const auto toTile = ePathFinderMap::posToTile(to);

    found = false;

    ePoint geoClosestTile = fromTile;
    const auto calcMinGeoDist = [&](const ePoint& from) {
        return std::max(std::abs(from.fX - toTile.fX),
                        std::abs(from.fY - toTile.fY));
    };
    int minGeoDist = calcMinGeoDist(fromTile);

    std::deque<ePoint> toProcess;
    const auto processTile = [&](const ePoint& from,
                                 const int dx, const int dy,
                                 const int dist) {
        if(dx == 0 && dy == 0) return;
        const bool r = map.walkable(from, dx, dy);
        if(!r) return;
        const ePoint to{from.fX + dx, from.fY + dy};
        const int geoDist = calcMinGeoDist(to);
        if(geoDist < minGeoDist) {
            geoClosestTile = to;
            minGeoDist = geoDist;
        }
        const int newDist = dist + 1;
        if(map.distance(to) > newDist) {
            map.setDistance(to, newDist);
            toProcess.push_back(to);
        }
        if(to == toTile) {
            found = true;
        }
    };

    const auto pathFinder = [&](const ePoint& from) {
        const int dist = map.distance(from);
        for(const int dx : {0, 1, -1}) {
            for(const int dy : {0, 1, -1}) {
                processTile(from, dx, dy, dist);
            }
        }
    };

    map.setDistance(fromTile, 0);
    toProcess.push_back(fromTile);
    while(!found && !toProcess.empty()) {
        const auto t = toProcess.front();
        toProcess.pop_front();
        pathFinder(t);
    }

    ePoint tile = found ? toTile : geoClosestTile;
    while(tile != fromTile) {
        ePoint minTile = tile;
        int minDist = map.distance(tile);
        for(const int dx : {0, 1, -1}) {
            for(const int dy : {0, 1, -1}) {
                if(dx == 0 && dy == 0) continue;
                const ePoint dtile{tile.fX + dx, tile.fY + dy};
                const int dist = map.distance(dtile);
                if(dist < minDist) {
                    minDist = dist;
                    minTile = dtile;
                }
            }
        }
        if(tile == minTile) return result;
        const auto minTilePos = ePathFinderMap::tileToPos(minTile);
        const auto tilePos = ePathFinderMap::tileToPos(tile);
        result.insert(result.begin(), {minTilePos, tilePos});
        tile = minTile;
    }

    return result;
}
