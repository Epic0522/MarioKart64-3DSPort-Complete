#include "input_policy_3ds.hpp"

#include <cassert>
#include <cstdio>

namespace {

using mk64_3ds::LegacyCInputPolicy3DS;
using mk64_3ds::MapLegacyCInput;
using mk64_3ds::MapPrimaryInput;
using mk64_3ds::ModalDismissAction3DS;
using mk64_3ds::PrimaryInputPolicy3DS;
using mk64_3ds::ResolveModalDismissAction;

void CheckMenuAndFilteredInput() {
    PrimaryInputPolicy3DS menu = {};
    menu.a = true;
    menu.b = true;
    menu.x = true;
    menu.select = true;
    menu.y = true;
    menu.start = true;
    menu.l = true;
    menu.r = true;
    menu.dUp = true;
    menu.dDown = true;
    menu.dLeft = true;
    menu.dRight = true;

    constexpr std::uint16_t kExpected =
        MK64_N64_A | MK64_N64_B | MK64_N64_CUP | MK64_N64_CLEFT |
        MK64_N64_START | MK64_N64_L | MK64_N64_R | MK64_N64_DUP |
        MK64_N64_DDOWN | MK64_N64_DLEFT | MK64_N64_DRIGHT;
    assert(MapPrimaryInput(menu) == kExpected);

    // A modal bottom-screen panel filters every game key before this policy.
    const PrimaryInputPolicy3DS filtered = {};
    const LegacyCInputPolicy3DS filteredC = {};
    assert(MapPrimaryInput(filtered) == 0);
    assert(MapLegacyCInput(filteredC) == 0);
}

void CheckRaceControls() {
    PrimaryInputPolicy3DS race = {};
    race.a = true;
    race.b = true;
    race.x = true;
    race.l = true;
    race.r = true;
    race.raceControls = true;

    assert(MapPrimaryInput(race) ==
           (MK64_N64_A | MK64_N64_B | MK64_N64_CLEFT |
            MK64_N64_CDOWN | MK64_N64_R));
    race.x=false;race.select=true;
    assert((MapPrimaryInput(race)&(MK64_N64_CUP|MK64_N64_CLEFT))==MK64_N64_CUP);

    LegacyCInputPolicy3DS suppressed = {};
    suppressed.zl = true;
    suppressed.zr = true;
    suppressed.cstickX = 127;
    suppressed.cstickY = -127;
    suppressed.raceControls = true;
    assert(MapLegacyCInput(suppressed) == 0);

    // ZL, ZR, and all four C-Stick directions stay non-digital in a race.
    suppressed.zr = false;
    suppressed.cstickX = 0;
    suppressed.cstickY = 0;
    assert(MapLegacyCInput(suppressed) == 0);
    suppressed.zl = false;
    suppressed.zr = true;
    assert(MapLegacyCInput(suppressed) == 0);
    suppressed.zr = false;
    suppressed.cstickX = -127;
    assert(MapLegacyCInput(suppressed) == 0);
    suppressed.cstickX = 0;
    suppressed.cstickY = 127;
    assert(MapLegacyCInput(suppressed) == 0);
}

void CheckBackupItemPolicy() {
    PrimaryInputPolicy3DS y = {}, l = {};
    y.y = true;
    l.l = true;
    y.raceControls = l.raceControls = true;
    assert(MapPrimaryInput(y) == MK64_N64_CDOWN);
    assert(MapPrimaryInput(y) == MapPrimaryInput(l));
    y.l = true;
    assert(MapPrimaryInput(y) == MK64_N64_CDOWN);
    y.raceControls = false;
    y.l = false;
    assert(MapPrimaryInput(y) == MK64_N64_CLEFT);
}

void CheckLegacyCButtons() {
    LegacyCInputPolicy3DS legacy = {};
    legacy.zl = true;
    assert(MapLegacyCInput(legacy) == 0);
    legacy = {};
    legacy.zr = true;
    assert(MapLegacyCInput(legacy) == MK64_N64_CRIGHT);
    legacy = {};
    legacy.cstickY = -71;
    assert(MapLegacyCInput(legacy) == MK64_N64_CDOWN);
    legacy.cstickY = 71;
    assert(MapLegacyCInput(legacy) == MK64_N64_CUP);
    legacy = {};
    legacy.cstickX = -71;
    assert(MapLegacyCInput(legacy) == MK64_N64_CLEFT);
    legacy.cstickX = 71;
    assert(MapLegacyCInput(legacy) == MK64_N64_CRIGHT);

    legacy.cstickX = 70;
    assert(MapLegacyCInput(legacy) == 0);
    legacy.cstickX = -70;
    assert(MapLegacyCInput(legacy) == 0);
}

void CheckModalDismissPolicy() {
    assert(ResolveModalDismissAction(false, false) == ModalDismissAction3DS::None);
    assert(ResolveModalDismissAction(false, true) == ModalDismissAction3DS::None);
    assert(ResolveModalDismissAction(true, false) == ModalDismissAction3DS::Close);
    assert(ResolveModalDismissAction(true, true) == ModalDismissAction3DS::Continue);

    // The action intentionally has no tab or row input. B/START and the
    // footer can therefore never activate MEMORY DUMP (or any other row)
    // merely because Developer happened to be selected.
    for (int tab = 0; tab < 4; ++tab) {
        for (int row = 0; row < 3; ++row) {
            (void) tab;
            (void) row;
            assert(ResolveModalDismissAction(true, true) ==
                   ModalDismissAction3DS::Continue);
        }
    }
}

} // namespace

int main() {
    CheckMenuAndFilteredInput();
    CheckRaceControls();
    CheckBackupItemPolicy();
    CheckLegacyCButtons();
    CheckModalDismissPolicy();
    std::puts("3DS input policy: ok");
    return 0;
}
