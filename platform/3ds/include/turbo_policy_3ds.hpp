#pragma once
#include <algorithm>

namespace mk64_3ds {
// Session-only latch. Modal/pause blocks activation, not the saved latch;
// the multiplier remains the existing persisted Gameplay setting.
struct TurboPolicy3DS {
    bool latched = false;
    void Reset() { latched = false; }
    int Update(int multiplier, bool paused, bool captured, bool zlPressed,
               bool cstickHeld) {
        multiplier = std::clamp(multiplier, 1, 5);
        if (multiplier == 1) latched = false;
        else if (!paused && !captured && zlPressed) latched = !latched;
        return !paused && !captured && (latched || cstickHeld) ? multiplier : 1;
    }
};
}
