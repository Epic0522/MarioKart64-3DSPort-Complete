#pragma once
#include <cstdint>

namespace mk64_3ds {
// Temporary test-build feature: one request per physical hold. A pause,
// attract demo, countdown or scene transition cancels rather than deferring it.
struct DebugFinishHold {
    std::uint64_t started = 0;
    bool timing = false, consumed = false;
    bool Update(bool held, bool eligible, std::uint64_t now) {
        if (!held || !eligible) { started = 0; timing = consumed = false; return false; }
        if (!timing) { started = now; timing = true; }
        if (consumed || now - started < 800) return false;
        consumed = true;
        return true;
    }
};
}
extern "C" void Mk64DebugFinish3DSSetHeld(bool held);
extern "C" void Mk64DebugFinish3DSTick(void);
