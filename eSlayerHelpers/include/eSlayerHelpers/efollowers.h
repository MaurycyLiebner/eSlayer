#ifndef EFOLLOWERS_H
#define EFOLLOWERS_H

#include "eunitspecialdata.h"

#include <map>
#include <set>

class ePacket;

struct eFollowersBase : public std::set<uint32_t> {
    void add(const uint32_t id);
    void remove(const uint32_t id);

    uint16_t fState = 0;

    void read(ePacket& p);
    void write(ePacket& p) const;
private:
    using B = std::set<uint32_t>;
    using B::erase;
    using B::emplace;
};

struct eFollowers {
    static std::map<uint32_t, eUnitSpecialData> sFollowers;
};

#endif // EFOLLOWERS_H
