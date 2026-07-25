#ifndef EPACKET_H
#define EPACKET_H

#include "eslayerhelpersexport.h"

#include "eexceptions.h"

#include <cstdint>
#include <cstring>
#include <limits>
#include <set>
#include <string>
#include <vector>

struct eTile;
class ESLAYERHELPERS_API ePacket {
public:
    const uint8_t* data() const { return buffer.data(); }
    size_t size() const { return buffer.size(); }

    void setData(const uint8_t* d, const size_t s) {
        buffer.assign(d, d + s);
        readPos = 0;
    }

    template<typename T>
    void peek(T& v) const {
        if(readPos + sizeof(T) > buffer.size()) {
            eRuntimeThrow("ePacket overflow");
        }

        memcpy(&v, buffer.data() + readPos, sizeof(T));
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

    ePacket& operator<<(const eTile& v) = delete;
    ePacket& operator>>(eTile& v) = delete;

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

    static uint8_t fromFloatU8(const float v, const float max);
    static float toFloatU8(const uint8_t v, const float max);
    static float roundFloatU8(const float v, const float max);

    static uint16_t fromFloatU16(const float v, const float max);
    static float toFloatU16(const uint16_t v, const float max);
    static float roundFloatU16(const float v, const float max);

    float readFloatU8(const float max);
    void writeFloatU8(const float v, const float max);

    float readFloatU16(const float max);
    void writeFloatU16(const float v, const float max);

    template <typename T>
    void write8(const std::vector<T>& v) {
        write<T, uint8_t>(v);
    }

    template <typename T>
    void write16(const std::vector<T>& v) {
        write<T, uint16_t>(v);
    }

    template <typename T>
    void read8(std::vector<T>& v) {
        read<T, uint8_t>(v);
    }

    template <typename T>
    void read16(std::vector<T>& v) {
        read<T, uint16_t>(v);
    }

    template <typename T>
    void write8(const std::set<T>& v) {
        write<T, uint8_t>(v);
    }

    template <typename T>
    void write16(const std::set<T>& v) {
        write<T, uint16_t>(v);
    }

    template <typename T>
    void read8(std::set<T>& v) {
        read<T, uint8_t>(v);
    }

    template <typename T>
    void read16(std::set<T>& v) {
        read<T, uint16_t>(v);
    }
private:
    template <typename T, typename U>
    void write(const std::vector<T>& v) {
        const U size = std::min<std::size_t>(std::numeric_limits<U>::max(), v.size());
        *this << size;
        for(U i = 0; i < size; i++) {
            *this << v[i];
        }
    }

    template <typename T, typename U>
    void read(std::vector<T>& v) {
        U size;
        *this >> size;
        v.reserve(v.size() + size);
        for(U i = 0; i < size; i++) {
            *this >> v.emplace_back();
        }
    }

    template <typename T, typename U>
    void write(const std::set<T>& v) {
        const U size = std::min<std::size_t>(std::numeric_limits<U>::max(), v.size());
        *this << size;
        U i = 0;
        for(const auto& t : v) {
            *this << t;
            if(++i >= size) break;
        }
    }

    template <typename T, typename U>
    void read(std::set<T>& v) {
        U size;
        *this >> size;
        for(U i = 0; i < size; i++) {
            T t;
            *this >> t;
            v.emplace(t);
        }
    }

    template<typename T>
    void write(const T& v) {
        const size_t pos = buffer.size();
        buffer.resize(pos + sizeof(T));
        memcpy(buffer.data() + pos, &v, sizeof(T));
    }

    template<typename T>
    void read(T& v) {
        peek(v);
        readPos += sizeof(T);
    }

    std::vector<uint8_t> buffer;
    size_t readPos = 0;
};

#endif // EPACKET_H
