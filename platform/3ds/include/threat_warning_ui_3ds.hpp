#pragma once
#include "threat_warning_3ds.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace mk64_3ds {
struct ThreatWarningTween3DS {
    float x = 136, reveal = 0, targetX = 136;
    uint8_t item = 0;
    bool active = false, clockStarted = false;
    uint64_t lastTick = 0;
    void Reset() { *this = {}; }
    void Target(const Mk64ThreatWarning3DS* threat) {
        active = threat != nullptr;
        if (!active) return;
        targetX = 136 + 128 * std::clamp(threat->bearing, -1.0f, 1.0f);
        if (reveal == 0) x = targetX;
        item = threat->item;
    }
    void Advance(uint64_t now) {
        const float dt = clockStarted && now >= lastTick ?
            static_cast<float>(std::min<uint64_t>(now-lastTick, 100)) : 0;
        lastTick = now; clockStarted = true;
        reveal = std::clamp(reveal + (active ? dt : -dt) / 160.0f, 0.0f, 1.0f);
        x += (targetX-x) * (1.0f-std::exp(-dt/80.0f));
    }
    bool Visible() const { return reveal > 0 && item != 0; }
    float Y() const { return -39 + 41 * reveal; }
    bool Pink() const { return (lastTick / 240) & 1; }
};
}
