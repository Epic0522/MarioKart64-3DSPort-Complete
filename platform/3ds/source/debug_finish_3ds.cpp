#include "debug_finish_3ds.hpp"
#include "diagnostics_3ds.h"
#include "settings_3ds.h"
#include <algorithm>
#include <cstring>
#include <libultraship.h>
extern "C" {
#include "main.h"
#include "defines.h"
#include "code_800029B0.h"
#include "code_80005FD0.h"
#include "waypoints.h"
#include "racing/collision.h"
}

namespace {
bool sHeld = false, sActive = false;
unsigned sVisited = 0;
mk64_3ds::DebugFinishHold sHold;
}
extern "C" void Mk64DebugFinish3DSSetHeld(bool held) { sHeld = held; }

extern "C" void Mk64DebugFinish3DSTick() {
    // GP/VS only: time trials must not acquire bogus best times/ghost saves.
    const bool eligible = Mk64Settings3DSGetZrDebug() &&
        gGamestate == RACING && gRaceState == RACE_IN_PROGRESS &&
        !gIsGamePaused && !gDemoMode && gPlayerCountSelection1 == 1 &&
        (gModeSelection == GRAND_PRIX || gModeSelection == VERSUS) &&
        (gPlayers[0].type & PLAYER_EXISTS) && !(gPlayers[0].type & PLAYER_CINEMATIC_MODE) &&
        gLapCountByPlayerId[0] < 3 && gTrackPaths[0] != nullptr && gPathCountByPathIndex[0] > 20;
    if (sHold.Update(sHeld, eligible, osGetTime())) {
        sActive = true;
        sVisited = 0;
        Mk64Diagnostics3DSCheckpoint("debug-zr-checkpoints-start");
    }
    if (!eligible) { sActive = false; return; }
    if (!sActive) return;

    Player& player = gPlayers[0];
    // Follow the current route in Yoshi Valley, rather than teleporting across
    // forks. Each adjacent point runs the ORIGINAL finish-line/path updater.
    // No direct lap/rank/race-state edits. Bound work and stop on bad progress.
    const int pathIndex = gPathIndexByPlayerId[0];
    if (pathIndex < 0 || pathIndex >= 4 || !gTrackPaths[pathIndex] ||
        gPathCountByPathIndex[pathIndex] <= 20) { sActive = false; return; }
    const auto count = gPathCountByPathIndex[pathIndex];
    const int previousPath = gPlayerPathIndex;
    gPlayerPathIndex = pathIndex;
    set_current_path(pathIndex);
    for (int step = 0; step < 128 && gLapCountByPlayerId[0] < 3; ++step) {
        const unsigned index = (gNearestPathPointByPlayerId[0] + 1U) % count;
        const auto& point = gTrackPaths[pathIndex][index];
        player.pos[0] = point.x;
        player.pos[1] = point.y + player.boundingBoxSize;
        player.pos[2] = point.z;
        std::memcpy(player.oldPos, player.pos, sizeof(player.pos));
        std::fill_n(player.velocity, 3, 0.0f);
        player.speed = 0.0f;
        if (point.trackSectionId != get_track_section_id(player.collision.meshIndexZX))
            check_bounding_collision(&player.collision, player.boundingBoxSize,
                player.pos[0], player.pos[1], player.pos[2]);
        update_player_path_completion(0, &player);
        if (++sVisited > static_cast<unsigned>(count) * 5U) {
            sActive = false;
            Mk64Diagnostics3DSCheckpoint("debug-zr-checkpoints-no-progress");
            break;
        }
    }
    // Refresh at section boundaries above, and at the final teleport before
    // normal physics resumes; not at every intermediate path point.
    check_bounding_collision(&player.collision, player.boundingBoxSize,
        player.pos[0], player.pos[1], player.pos[2]);
    set_places();
    gPlayerPathIndex = previousPath;
    set_current_path(previousPath);
    if (gLapCountByPlayerId[0] >= 3) {
        sActive = false;
        Mk64Diagnostics3DSCheckpoint("debug-zr-checkpoints-finished");
    }
}
