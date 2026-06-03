#include "edungeongenerator.h"

void eDungeonGenerator::generate(
    const eRect& rect,
    std::vector<eChamber>& chambers,
    std::vector<eRect>& doors) {
    const int roomSize = 6;
    const int connThick = 2;
    const int connLen = 4;

    enum class eDir {
        none,
        topLeft, topRight,
        bottomRight, bottomLeft
    };

    struct eRoom {
        int fRelX;
        int fRelY;

        int fAbsX;
        int fAbsY;

        bool fEnabled = false;

        void enableInDir(const eDir dir) {
            switch(dir) {
            case eDir::none:
                return;
            case eDir::topLeft:
                fTLConn = true;
                return;
            case eDir::topRight:
                fTRConn = true;
                return;
            case eDir::bottomRight:
                fBRConn = true;
                return;
            case eDir::bottomLeft:
                fBLConn = true;
                return;
            }
        }

        bool fTLConn = false;
        bool fTRConn = false;
        bool fBRConn = false;
        bool fBLConn = false;
    };

    const int roomRectDim = roomSize + connLen;
    const int xNRooms = rect.fW/roomRectDim;
    const int yNRooms = rect.fH/roomRectDim;
    std::vector<std::vector<eRoom>> rooms(
        yNRooms, std::vector<eRoom>(xNRooms));
    for(int x = 0; x < xNRooms; x++) {
        for(int y = 0; y < yNRooms; y++) {
            auto& r = rooms[y][x];
            r.fRelX = x;
            r.fRelY = y;
            r.fAbsX = rect.fX + x*roomRectDim;
            r.fAbsY = rect.fY + y*roomRectDim;
        }
    }

    const auto hasRoom = [&](const int relX, const int relY) {
        return relX >= 0 && relX < xNRooms &&
               relY >= 0 && relY < yNRooms;
    };

    const auto flipDir = [](const eDir from) {
        switch(from) {
        case eDir::none:
            return eDir::none;
        case eDir::topLeft:
            return eDir::bottomRight;
        case eDir::topRight:
            return eDir::bottomLeft;
        case eDir::bottomRight:
            return eDir::topLeft;
        case eDir::bottomLeft:
            return eDir::topRight;
        }
        return eDir::none;
    };

    const auto moveInDir = [](const eDir dir,
                              int& relX, int& relY) {
        switch(dir) {
        case eDir::none:
            return;
        case eDir::topLeft:
            relX--;
            return;
        case eDir::topRight:
            relY--;
            return;
        case eDir::bottomRight:
            relX++;
            return;
        case eDir::bottomLeft:
            relY++;
            return;
        }
    };

    const int firstRelX = rect.fW/roomRectDim/2;
    const int firstRelY = rect.fH/roomRectDim/2;

    rooms[firstRelY][firstRelX].fEnabled = true;

    const auto generateRandomPath = [&]() {
        int relX = firstRelX;
        int relY = firstRelY;
        eDir dir = eDir::none;

        for(int i = 0; i < 10; i++) {
            const auto excl = flipDir(dir);
            bool hasRoomB = false;

            int newRelY = relY;
            int newRelX = relX;
            do {
                const auto idir = eRand::rand(1, 4);
                dir = static_cast<eDir>(idir);

                newRelY = relY;
                newRelX = relX;
                moveInDir(dir, newRelX, newRelY);
                hasRoomB = hasRoom(newRelX, newRelY);
            } while(dir == excl || !hasRoomB);

            rooms[relY][relX].enableInDir(dir);
            const auto fdir = flipDir(dir);
            auto& newRoom = rooms[newRelY][newRelX];
            newRoom.fEnabled = true;
            newRoom.enableInDir(fdir);

            relY = newRelY;
            relX = newRelX;
        }
    };

    for(int i = 0; i < 5; i++) {
        generateRandomPath();
    }

    const auto addBRConn = [&](const eRoom& room) {
        if(room.fBRConn) {
            {
                const eRect rect{room.fAbsX + roomSize,
                                 room.fAbsY + (roomSize - connThick)/2,
                                 0, connThick};
                doors.emplace_back(rect);
            }
            const eRect rect{room.fAbsX + roomSize,
                             room.fAbsY + (roomSize - connThick)/2,
                             connLen, connThick};
            chambers.emplace_back(rect);
            {
                const eRect rect{room.fAbsX + roomSize + connLen,
                                 room.fAbsY + (roomSize - connThick)/2,
                                 0, connThick};
                doors.emplace_back(rect);
            }
        }
    };

    const auto addBLConn = [&](const eRoom& room) {
        if(room.fBLConn) {
            {
                const eRect rect{room.fAbsX + (roomSize - connThick)/2,
                                 room.fAbsY + roomSize,
                                 connThick, 0};
                doors.emplace_back(rect);
            }
            const eRect rect{room.fAbsX + (roomSize - connThick)/2,
                             room.fAbsY + roomSize,
                             connThick, connLen};
            chambers.emplace_back(rect);
            {
                const eRect rect{room.fAbsX + (roomSize - connThick)/2,
                                 room.fAbsY + roomSize + connLen,
                                 connThick, 0};
                doors.emplace_back(rect);
            }
        }
    };

    const auto trySquareMerge = [&](
                                    const eRoom& room) {
        if(room.fRelY >= yNRooms - 1) return false;
        if(room.fRelX >= xNRooms - 1) return false;
        if(!room.fBRConn) return false;
        if(!room.fBLConn) return false;
        auto& roomBR = rooms[room.fRelY][room.fRelX + 1];
        if(!roomBR.fEnabled) return false;
        if(!roomBR.fBLConn) return false;
        auto& roomBL = rooms[room.fRelY + 1][room.fRelX];
        if(!roomBL.fEnabled) return false;
        if(!roomBL.fBRConn) return false;
        auto& roomB = rooms[room.fRelY + 1][room.fRelX + 1];
        if(!roomB.fEnabled) return false;
        roomBR.fEnabled = false;
        roomBL.fEnabled = false;
        roomB.fEnabled = false;
        auto& c = chambers.emplace_back();
        const int dim1 = 2*roomSize + connLen;
        const int dim2 = 2*roomSize/3;
        const int d = roomSize - dim2;
        const int dim3 = connLen + 2*d;
        auto& rects = c.fRects;
        rects.emplace_back(room.fAbsX,
                           room.fAbsY,
                           dim1, dim2);
        rects.emplace_back(room.fAbsX,
                           room.fAbsY + dim2,
                           dim2, dim3);
        rects.emplace_back(roomBR.fAbsX + d,
                           roomBR.fAbsY + dim2,
                           dim2, dim3);
        rects.emplace_back(roomBL.fAbsX,
                           roomBL.fAbsY + d,
                           dim1, dim2);
        addBRConn(roomBR);
        addBLConn(roomBL);
        addBRConn(roomB);
        addBLConn(roomB);
        return true;
    };

    const auto tryCrossMerge = [&](const eRoom& room) {
        if(room.fRelY >= yNRooms - 2) return false;
        if(room.fRelX >= xNRooms - 2) return false;
        if(!room.fBLConn) return false;
        auto& roomBL = rooms[room.fRelY + 1][room.fRelX];
        if(!roomBL.fEnabled) return false;
        if(!roomBL.fBLConn) return false;
        if(!roomBL.fBRConn) return false;
        if(!roomBL.fTLConn) return false;
        auto& roomBLBL = rooms[room.fRelY + 2][room.fRelX];
        if(!roomBLBL.fEnabled) return false;
        auto& roomBLBR = rooms[room.fRelY + 1][room.fRelX + 1];
        if(!roomBLBR.fEnabled) return false;
        auto& roomBLTL = rooms[room.fRelY + 1][room.fRelX - 1];
        if(!roomBLTL.fEnabled) return false;
        roomBL.fEnabled = false;
        roomBLBL.fEnabled = false;
        roomBLBR.fEnabled = false;
        roomBLTL.fEnabled = false;
        auto& c = chambers.emplace_back();
        const int dim1 = 2*roomSize + connLen;
        const int dim2 = 2*roomSize/3;
        const int d = roomSize - dim2;
        const int dim3 = connLen + 2*d;
        auto& rects = c.fRects;
        rects.emplace_back(room.fAbsX, room.fAbsY,
                           roomSize, 3*roomSize + 2*connLen);
        rects.emplace_back(roomBLTL.fAbsX, roomBLTL.fAbsY,
                           roomSize + connLen, roomSize);
        rects.emplace_back(roomBLBR.fAbsX - connLen, roomBLBR.fAbsY,
                           roomSize + connLen, roomSize);
        addBRConn(roomBLBL);
        addBLConn(roomBLBL);
        addBRConn(roomBLBR);
        addBLConn(roomBLBR);
        addBLConn(roomBLTL);
        return true;
    };

    for(const auto& row : rooms) {
        for(const auto& room : row) {
            if(!room.fEnabled) continue;
            const bool r = tryCrossMerge(room);
            if(r) continue;
            const bool rr = trySquareMerge(room);
            if(rr) continue;
            addBRConn(room);
            addBLConn(room);
            const eRect rect{room.fAbsX, room.fAbsY,
                             roomSize, roomSize};
            chambers.emplace_back(rect);
        }
    }
}
