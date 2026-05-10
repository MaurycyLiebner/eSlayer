#include "eSlayerHelpers/epacket.h"

#include <cmath>
#include <limits>

const float u8max = std::numeric_limits<uint8_t>::max();
const float u16max = std::numeric_limits<uint16_t>::max();

uint8_t ePacket::fromFloatU8(const float v, const float max) {
    return std::clamp(std::round(u8max / max * v), 0.f, u8max);
}

float ePacket::toFloatU8(const uint8_t v, const float max) {
    return std::clamp(max / u8max * v, 0.f, max);
}

uint16_t ePacket::fromFloatU16(const float v, const float max) {
    return std::clamp(std::round(u16max / max * v), 0.f, u16max);
}

float ePacket::toFloatU16(const uint16_t v, const float max) {
    return std::clamp(max / u16max * v, 0.f, max);
}

float ePacket::readFloatU8(const float max) {
    uint8_t v;
    *this >> v;
    return toFloatU8(v, max);
}

void ePacket::writeFloatU8(const float v, const float max) {
    *this << fromFloatU8(v, max);
}

float ePacket::readFloatU16(const float max) {
    uint16_t v;
    *this >> v;
    return toFloatU16(v, max);
}

void ePacket::writeFloatU16(const float v, const float max) {
    *this << fromFloatU16(v, max);
}
