#ifndef EPACKET_H
#define EPACKET_H

#include "eslayerhelpersexport.h"

#include "eSlayerHelpers/eexceptions.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

class ESLAYERHELPERS_API ePacket {
public:
    const uint8_t* data() const { return buffer.data(); }
    size_t size() const { return buffer.size(); }

    void setData(const uint8_t* d, const size_t s) {
        buffer.assign(d, d + s);
        readPos = 0;
    }

    template<typename T>
    ePacket& operator<<(const T& v) {
        write(v);
        return *this;
    }

    template<typename T>
    ePacket& operator>>(T& v) {
        read(v);
        return *this;
    }

    ePacket& operator<<(const std::string& str) {
        const uint32_t len = str.size();
        write(len);

        const size_t pos = buffer.size();
        buffer.resize(pos + len);
        memcpy(buffer.data() + pos, str.data(), len);

        return *this;
    }

    ePacket& operator>>(std::string& str) {
        uint32_t len;
        read(len);

        if(readPos + len > buffer.size()) {
            eRuntimeThrow("ePacket overflow");
        }

        str.assign((char*)buffer.data() + readPos, len);
        readPos += len;

        return *this;
    }

private:
    template<typename T>
    void write(const T& v) {
        const size_t pos = buffer.size();
        buffer.resize(pos + sizeof(T));
        memcpy(buffer.data() + pos, &v, sizeof(T));
    }

    template<typename T>
    void read(T& v) {
        if(readPos + sizeof(T) > buffer.size()) {
            eRuntimeThrow("ePacket overflow");
        }

        memcpy(&v, buffer.data() + readPos, sizeof(T));
        readPos += sizeof(T);
    }

    std::vector<uint8_t> buffer;
    size_t readPos = 0;
};

#endif // EPACKET_H
