#include "../include/eSlayerHelpers/epathfinder.h"

#include <cassert>
#include <cstdlib>
#include <deque>
#include <limits>

struct ePathFinderBoardTile {
    int fDist = std::numeric_limits<int>::max();
};

class ePathFinderBoard : public std::vector<std::vector<ePathFinderBoardTile>> {
  public:
    ePathFinderBoard(const int w, const int h) :
        mWidth(w), mHeight(h) {
        resize(h);
        for(int y = 0; y < h; y++) {
            operator[](y).resize(w);
        }
    }

    void set(const ePoint& p, const int v) {
        operator[](p.fY)[p.fX].fDist = v;
    }

    int get(const ePoint& p) const {
        if(p.fX < 0 || p.fX >= mWidth) return std::numeric_limits<int>::max();
        if(p.fY < 0 || p.fY >= mHeight) return std::numeric_limits<int>::max();
        return operator[](p.fY)[p.fX].fDist;
    }
private:
    const int mWidth;
    const int mHeight;
};

ePathFinderPath ePathFinder::findPath(
    const ePathFinderMap& map,
    const ePoint& from,
    const ePoint& to,
    bool& found) {
    found = false;

    ePathFinderPath result;
    if(from == to) {
        found = true;
        return result;
    }

    const int w = map.width();
    const int h = map.height();
    ePathFinderBoard board(w, h);

    const auto canWalk = [&](const ePoint& tile,
                             const int dx,
                             const int dy) {
        if(!map.get(tile)) return false;
        if(dx != 0 && dy != 0) {
            std::pair<ePoint, ePoint> surr;
            if(dx == -1 && dy == -1) {
                surr.first = ePoint{tile.fX, tile.fY + 1};
                surr.second = ePoint{tile.fX + 1, tile.fY};
            } else if(dx == -1 && dy == 1) {
                surr.first = ePoint{tile.fX, tile.fY - 1};
                surr.second = ePoint{tile.fX + 1, tile.fY};
            } else if(dx == 1 && dy == 1) {
                surr.first = ePoint{tile.fX, tile.fY - 1};
                surr.second = ePoint{tile.fX - 1, tile.fY};
            } else { // if(dx == 1 && dy == -1) {
                surr.first = ePoint{tile.fX, tile.fY + 1};
                surr.second = ePoint{tile.fX - 1, tile.fY};
            }
            if(!map.get(surr.first)) return false;
            if(!map.get(surr.second)) return false;
        }
        return true;
    };

    ePoint geoClosestTile = from;
    const auto calcMinGeoDist = [&](const ePoint& from) {
        return std::max(std::abs(from.fX - to.fX),
                        std::abs(from.fY - to.fY));
    };
    int minGeoDist = calcMinGeoDist(from);

    std::deque<ePoint> toProcess;
    const auto processTile = [&](const ePoint& tile,
                                 const int dx, const int dy,
                                 const int dist) {
        if(dx == 0 && dy == 0) return;
        const bool r = canWalk(tile, dx, dy);
        if(!r) return;
        const int geoDist = calcMinGeoDist(tile);
        if(geoDist < minGeoDist) {
            geoClosestTile = tile;
            minGeoDist = geoDist;
        }
        const int newDist = dist + 1;
        if(board.get(tile) > newDist) {
            board.set(tile, newDist);
            toProcess.push_back(tile);
        }
        if(tile == to) {
            found = true;
        }
    };

    const auto pathFinder = [&](const ePoint& from) {
        for(const int dx : {0, 1, -1}) {
            for(const int dy : {0, 1, -1}) {
                const int dist = board.get(from);
                const ePoint to{from.fX + dx, from.fY + dy};
                processTile(to, dx, dy, dist);
            }
        }
    };

    board.set(from, 0);
    toProcess.push_back(from);
    while(!found && !toProcess.empty()) {
        const auto t = toProcess.front();
        toProcess.pop_front();
        pathFinder(t);
    }

    ePoint tile = geoClosestTile;
    ePoint minTile = tile;
    int minDist = board.get(tile);
    while(tile != from) {
        for(const int dx : {0, 1, -1}) {
            for(const int dy : {0, 1, -1}) {
                if(dx == 0 && dy == 0) continue;
                const ePoint dtile{tile.fX + dx,
                                   tile.fY + dy};
                const bool r = canWalk(dtile, dx, dy);
                if(!r) continue;
                const int dist = board.get(dtile);
                if(dist < minDist) {
                    minDist = dist;
                    minTile = dtile;
                }
            }
        }
        assert(tile != minTile);
        result.insert(result.begin(), {minTile, tile});
        tile = minTile;
    }

    return result;
}
