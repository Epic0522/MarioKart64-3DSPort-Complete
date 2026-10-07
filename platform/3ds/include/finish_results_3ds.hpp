#pragma once
#include "hud_layout_3ds.hpp"
#include <algorithm>
#include <cstdint>

namespace mk64_3ds {
// A virtual hinge separates the displays: top occupies y=[0,240), lower starts
// at y=304. This is an animation design gap, not a model-specific measurement.
// Draw only each screen's window; the physical hinge is an unseen travel span.
class FinishResults3DS {
public:
    static constexpr float kHingeGap = 64;
    static constexpr float kLowerOriginY = 240 + kHingeGap;
    static constexpr uint64_t kFlightMilliseconds = 1040;
    float progress = 0;
    bool active = false;
    bool nativeOwned = false;
    uint64_t last = 0;
    uint64_t elapsedMs = 0;
    void Reset() {
        progress = 0;
        active = false;
        nativeOwned = false;
        last = elapsedMs = 0;
    }
    void Update(bool allowed, bool nativeTransfer, bool paused, uint64_t now,
                bool nativeLineup = false, unsigned nativePhase = 0) {
        if (!allowed) { Reset(); return; }
        // 0x7e ends native lineup entry. From this point the first four use
        // that same native row, including 0x8c..0x9c exit and hidden sentinels.
        // Latch ownership: nativePhase returns to zero after completing exit.
        if (nativeLineup && nativePhase >= 0x7e) nativeOwned = true;
        if (!nativeTransfer && !active) { last=now; return; }
        if (!active) { active=true; last=now; return; }
        if (!paused && now>=last) {
            // Integer duration reaches the endpoint exactly. Float accumulation
            // can leave progress just below one and suppress the lap breakdown.
            elapsedMs = std::min(kFlightMilliseconds, elapsedMs + std::min(now - last, uint64_t{100}));
            progress = elapsedMs / float(kFlightMilliseconds);
        }
        last=now;
    }
    HudRect Portrait(unsigned rank, bool top) const {
        const auto start=BottomStandingRect(rank);
        // Rear column exits first. Delay the front fan until it has cleared
        // their path, then lift through the unseen hinge to the native row.
        const auto ease = [](float t) { return t*t*(3-2*t); };
        const float ms = progress * kFlightMilliseconds;
        if (rank >= 4) {
            HudRect box{start.x+40,start.y+kLowerOriginY+256*ease(std::clamp(ms/300.f,0.f,1.f)),40,40};
            if (!top) {box.x-=40;box.y-=kLowerOriginY;}
            return box;
        }
        const float rise = 16*ease(std::clamp(ms/240.f,0.f,1.f));
        const float fan = ease(std::clamp((ms-240)/320.f,0.f,1.f));
        const float lift = ease(std::clamp((ms-560)/480.f,0.f,1.f));
        const float liftedStart = start.y+kLowerOriginY-56;
        HudRect box{start.x+40+(68.f+rank*32-(start.x+40))*fan,
                    start.y+kLowerOriginY-rise-40*fan+(94.f-liftedStart)*lift,
                    start.width+(32-start.width)*fan,start.height+(32-start.height)*fan};
        if (!top) {box.x-=40;box.y-=kLowerOriginY;}
        return box;
    }
    static bool Onscreen(HudRect box,float width) {
        return box.x<width && box.x+box.width>0 && box.y<240 && box.y+box.height>0;
    }
};
}
