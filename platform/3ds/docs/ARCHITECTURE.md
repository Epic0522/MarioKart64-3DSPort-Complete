# 3DS code map

Mario Kart 64 3DS Port Complete retains SpaghettiKart's game core. Platform runtime code and
checked adaptations are separate from the pinned third-party source. This is
an ownership/maintenance guide, not a claim of a new engine implementation.

## Workspace boundaries

`MarioKart64-3DSPort-Complete` is the consolidated root and sole release entry
point. `experiments/native/` preserves the independent native-runtime experiment;
its CMake project, modified vendor snapshot and tests are deliberately separate.
Historical `presentation/` files there are not linked into either game target.
The experiment, `tests/`, validation receipts and banner preview/authoring tools
remain local-only and are excluded from the public source. Test paths below
refer to the maintainer's local suite; fresh clones need only the public game
modules and `platform/3ds/check_release_source.py` packaging guard.
The root now owns its `.git/` directory. The preceding worktrees and complete
shared histories are archived outside the project; no production build depends
on that archive or the removed `local-reference/` tree. Pinned dependencies use
self-contained shallow stores and retain their upstream URLs and commit IDs.
Do not flatten conflicting files from those references into the accepted runtime.

## Where to change a feature

Paths below are relative to `platform/3ds/`.

| Responsibility | Runtime / policy | Adaptation / regression entry points |
| --- | --- | --- |
| Startup heap partition | `source/heap_partition_3ds.cpp`, `include/heap_policy_3ds.hpp` | `tests/heap_partition_test.cpp`, `tests/resource-host/` |
| Compressed archive and decoded owners | `source/o2r_archive_reader.cpp`, `source/resource_runtime_3ds.cpp`, `source/resource_manager_3ds.cpp` | `cmake/racing_resources_3ds.cmake`, `tests/archive_streaming_test.cpp`, `tests/archive_fragmentation_test.cpp`, `tests/resource-host/` |
| Scene and camera lifetime | `source/game_runtime_3ds.cpp` | `cmake/world_lifetime_3ds.cmake`, `cmake/racing_ending_3ds.cmake`, `tests/run-scene-transition-test.py` |
| Renderer / texture / combiner safety | `source/gfx_citro3d.cpp`, `include/combiner_cache_3ds.hpp`, `include/native_geometry_3ds.hpp` | `cmake/renderer_3ds.cmake`, `cmake/interpreter_patches_3ds.cmake`, `tests/run-texture-cache-test.py`, `tests/run-combiner-cache-test.py` |
| UI, minimaps and settings | `source/bottom_ui_3ds.cpp`, `source/settings_3ds.cpp`, `include/hud_layout_3ds.hpp` | `cmake/menu_details_3ds.cmake`, `cmake/time_trial_hud_3ds.cmake`, `tests/run-menu-ui-test.py`, `tests/run-ui-controls-test.py`, `tests/run-menu-art-test.py` |
| Input and independent clock | `source/input_3ds.cpp`, `include/input_policy_3ds.hpp`, `source/race_clock_3ds.cpp` | `tests/run-timing-test.py`, `tests/repro-race-clock-carryover.cpp` |
| Modern CPU / items / pace | `include/cpu_items_3ds.inc.h`, `include/cpu_leader_3ds.inc.h`, `include/item_hit_protection_3ds.inc.h`, `include/road_shells_3ds.inc.h`, `include/cpu_box_seek_3ds.inc.h` | `cmake/cpu_items_3ds.cmake`, `cmake/item_hit_protection_3ds.cmake`, `tests/run-item-policy-test.py`, `tests/run-leader-hit-test.py`, `tests/run-race-box-test.py` |
| AUTO and watched-driver feedback | `source/auto_mode_3ds.cpp`, `include/auto_audio_3ds.inc.hpp`, `include/cpu_obstacle_3ds.inc.hpp` | `cmake/auto_mode_3ds.cmake`, `tests/run-auto-mode-test.py`, `tests/run-auto-polish-test.py`, `tests/run-auto-feedback-test.py` |
| Solo VS/Battle, arena AI and balloon standings | `source/solo_modes_3ds.cpp`, `include/arena_navigation_3ds.hpp`, `include/battle_standings_3ds.hpp` | `cmake/solo_modes_3ds.cmake`, `cmake/world_tracks_bridge_3ds.cmake`, `tests/run-solo-modes-test.py`, `tests/run-battle-recovery-test.py`, `tests/run-solo-feedback-test.py` |
| Global Turbo toggle and accelerated clock | `include/turbo_policy_3ds.hpp`, `include/race_clock_3ds.hpp`, `source/race_clock_3ds.cpp` | `source/bottom_ui_3ds.cpp`, `tests/run-auto-solo-turbo-test.py`, `tests/run-timing-test.py` |
| Audio production / native effect requests | `source/audio_runtime_3ds.cpp`, `source/audio_ndsp_3ds.cpp`, `include/audio_feedback_3ds.h` | `cmake/racing_audio_menu_3ds.cmake`, `cmake/spatial_audio_3ds.cmake`, `tests/run-tree-audio-test.py`, `tests/run-feedback-test.py` |
| Approved HOME asset / package contract | `assets/banner/`, `build.sh`, `package_hashes.py` | `tests/run-title-metadata-test.py`, `tests/run-release-boundary-test.py` |

## Ownership rules

1. **Race boundaries:** tear down scene/camera/render references before releasing
   decoded course and kart resources. Same-course retry is a real boundary.
   Clear stale interpolation state; do not indiscriminately discard live audio
   or menus with unrelated ownership.
2. **GPU lifetime:** submitted texture backing survives until GPU completion.
   VBlank alone is not proof of completion. Cache pressure uses fenced batches,
   not hidden racers or partial frame presentation.
3. **Archive pressure:** complete residency and bounded compressed hot ranges
   are optional. Allocation may surrender them; pointers into residency must be
   resolved after reclaiming allocations. Cache mutation must not reenter its
   own reclamation. The shared streaming budget is not a second allocation.
4. **Gameplay switch:** modern logic has a preserved OFF path. Stability, UI,
   rendering and audio repairs do not depend on this switch. AUTO changes the
   driver controller, not the meaning of selected-driver cup ownership.
5. **Audio ABI:** pending-request capacity is distinct from native bank/source
   playback layout and concurrent voices. Do not enlarge original arrays by
   changing a channel macro without auditing every native indexing contract.

## Generated adaptations

Root `CMakeLists.txt` includes target modules in dependency order. The core,
engine and renderer groups include their scoped adaptation modules. Keep order
and replacement-point assertions intact: an unexpected upstream revision must
fail configuration rather than silently omit a patch.

Change the checked-in module or policy, then regenerate. Never hand-edit
`build-3ds/game/generated/` as the permanent fix. Dependency fixes needed for
a clean checkout belong in a reproducible checked-in patch; a dirty local
submodule is not a release dependency contract.

## Verification and publishing

`solo_modes_3ds.cpp` owns the added one-player mode lookups, VS grid and arena
bot state. `cmake/solo_modes_3ds.cmake` adapts generated menu, participant,
physics and result dispatch; the original `[4][3]` menu table ABI and GP/TT
storage remain intact. `arena_navigation_3ds.hpp` is an allocation-free graph
with a fixed 2,048-triangle capacity, six links per triangle, height-aware
portals and directed safe drops, plus 512 bounded wall triangles. Deduplicate
two-sided/repeated triangles at load. Build it only at arena load/retry; bot
objectives are staggered at 2 Hz and body-width steering at 10 Hz (short portal
refreshes have a 50 ms minimum). A fixed 16x16 bitset index adds 64 KiB and
restricts support queries to nearby floor triangles, without per-frame allocation.
Advance inventory once per course-clock image, not per physics substep. Look ahead
along the route only through continuously supported, wall-free segments;
also check the current heading and momentum near edges, including boosted motion.
Allow a short drop only when a floor actually exists below every probe; never
enable drop tolerance on Big Donut. Retain a supported recovery direction when
the kart's body already overlaps a ledge. Empty hands prioritize physical boxes,
with peer reservation penalties; prolonged close combat selects a stable,
separated waypoint instead of indefinitely orbiting an opponent.
Stable obstruction recovery must
not replace the objective every frame. Native physics, collision and balloons retain
authority. Overflow fails closed, rather than corrupting navigation storage.
Solo Battle requires box-driven inventory and spatial targeting even with
modern logic off; this does not enable modern race pacing or road shells.

Collision flags' low byte is a section ID. The native arenas use `0xFF`;
never reinterpret its `0x18` bits as a walkability exclusion. Tests must use
the real section flags, not zero-initialized substitutes. Navigation pauses
on `gIsGamePaused`, not the unrelated camera/cinematic state `D_801646CC`.
CPU bomb conversion retains CPU ownership; spent human bombs retain controller
slot zero for pause/quit while the camera follows a living bot. Never lend a
bot the human controller or end Battle merely because the human is eliminated.
Solo VS reuses GP finish audio through a scheduler-local mode value, not a
global game-mode swap; bomb contacts use eight participants, not one viewport.
`tests/run-solo-modes-test.py` and `tests/run-solo-feedback-test.py` exercise
these generated/native boundaries with sanitizers.

Battle shells wait for an opponent inside the native firing cone and a clear
wall-tested shot. Do not shorten the native triple-shell assembly/shot interval.
Solo Battle schedules human-relative win/loss short/long result sequence pairs
and the human's tick-30 voice on listener zero. Its result-menu delay allows the
short sequence to play. Solo VS/Battle exit cancels result state, resets its
clock, clears deferred setup commands for both sequence players and shortens
their fade to complete before the black menu transition. Native multiplayer
and menu fades remain unchanged.
The actual VS/Battle result menu (`func_800ACC50`) must call this cleanup
before starting its 30-tick screen fade, for Retry, Course Change, Driver
Change and Quit. Testing the fade helper directly does not verify this wiring.
The native result score is a cumulative human win count, not finishing place;
solo presentation omits only the unused two-human separator.
Eliminated CPU bombs keep chasing reachable survivors, including at contact
range. Ordinary combat spacing and stale roulette state cannot override that
objective. Supported-floor/wall probes and real obstruction recovery remain.
Battle box ranking uses the existing BFS route's cached length, not Euclidean
distance across stacked floors. This adds one fixed 8 KiB array, no allocation
or additional traversal per box. Retain pickup targets with modest hysteresis;
six seconds without meaningful approach temporarily rejects that box for eight
seconds and permits obstruction recovery. Ordinary combat spacing applies only
to visible opponents within 25 vertical units. It cannot make a bot repeatedly
abandon a ramp because another kart is above/below it. Empty hands still seek
boxes during Star; bomb contact objectives never seek boxes.
Character confirmation is UI audio: dispatch its original sample through
`play_sound2` outside racing, without reading stale racer effects/CPU state.
Other racing voices retain their original local/spatial routing and gain.

Finish portrait motion includes a 64-logical-pixel hidden hinge band. It is a
presentation allowance, not a measured universal hinge size. Result-only
depths place portraits over the enlarged rank and under transition overlays;
ordinary racing depths and native lineup/exit positions remain unchanged.
The lower grid uses two 40-pixel columns at x=32/90. Rear-four descend in
300 ms, clearing the path before the delayed front fan and lift complete at
1040 ms. The original native row owns all eight portraits after phase 0x7e,
including exit/sentinel handling. There is no lower large-place draw path.
Damage angles are separate snapshot fields, leaving real minimap headings
intact. Portrait, observed border and rank numeral rotate rigidly around one
center, without a fixed fill or per-slot clipping. Minimap opponents share the
native 8x8 purple dot; a cached white variant preserves its black contour and
alpha. Draw the full observed kart last and above dots. AUTO crops the four
approved letters from the existing hint, using the native 16-on/8-off cadence.

Ceremony uses the shared menu-transition observer with native ENDING enabled,
including its normal exit and Skip path. Staff lower-screen blanking checks
live state, pre-dispatch snapshot and the native `255` reload with next=`9`.
Before each lower transfer, that policy clears the retained target black and
bypasses all lower draws/overlays. Loading artwork uses the same policy. Do not
blank unrelated sentinel loads or alter the native upper-screen transition.

The input/UI snapshot precedes native scene dispatch. Race transition masks
must use the current scene when drawing, fully covering a stale menu snapshot
on the first race image. Close shutters at course-resource preparation and
force zero reveal during RACE_INIT; subsequent reveal follows the native fraction.
Apply retained native exit opacity to the custom upper HUD, which is submitted
after Fast3D's fade, so held-black frames cannot expose a flashing rank.
An opaque upper black-hold cover is submitted after every HUD/status draw;
the first menu image also masks a stale lower race snapshot, including timer
restoration. `frame_begin_3ds.hpp` fences the GPU queue and then checks both
actual GSP pending presents: VBlank counters may have advanced before the GPU
completion callback scheduled a newer swap, and are not sufficient alone.

Native EUC-JP decoding explicitly casts its lead byte to `s8`; ARM plain char
is unsigned. Otherwise two-byte score digits become separate C/B glyphs.
No-record labels use ASCII dashes, record IDs are bounded and Time Trial record
panels are mode-guarded. These are presentation fixes, not save-data rewrites.
`tests/run-record-label-test.py` executes the decoder with unsigned plain char.

Use [BUILDING.md](../../../BUILDING.md) for host and device-build commands;
[validation receipts](validation/README.md) route to feature-specific evidence.
Keep three levels separate: host regression, package inspection, hardware.

`tests/run-hud-animation-test.py` covers the native slide/scale/flash contracts
and production UI draw paths. Manual snapshots sample original object/HUD state;
AUTO uses eight presentation-only states from `include/hud_layout_3ds.hpp`,
advanced once per simulation preparation and reset at every race boundary.
Damage portraits have separate per-driver `PortraitSpin3DS` states, advanced
in both manual and AUTO races, frozen during pause, and settled on whole-turn
boundaries. Snapshot reads and midpoint images never advance that state.
`Mk64GameState3DSRefreshStandings` samples rank IDs, their actual characters
and native border colors after simulation and before drawing. A small
`StandingsPresentation3DS` cache key invalidates the retained lower target only
when these visible fields change; unchanged map caching remains bounded.
Opponent dots retain their native textures and centers at 120% drawing size.
The observed leader also alternates its 8x8 full kart with a cached white
variant, preserving alpha/black edges and heading; Time Trials do not flash.
`AutoConfirmPulse3DS` supplies local A-edge equivalents to native GP results,
poor-finish Retry, solo VS/Battle results and ceremony confirmation. It uses
500 ms of wall time, never simulation ticks, so Turbo cannot compress the
interval. GP pulses also use the original score fast-forward gates. Not-ready,
pause and fade reset the timer; race/session preparation clears all pulses.
Controller state is never mutated, and native entry/exit/readiness still own
transitions. Keep AUTO save suppression independent of these presentation gates.
The upper finish-place gate samples native `D_8015F894` before results-window
conversion; lower HUD visibility is independent of that gate. Generated menu
hooks forward the native opening fraction and signed results-exit pixel steps
to `RaceTransition3DS`. Both full lower redraws and live-timer redraws compose
the same transition overlay. The harness executes the generated GP exit updater
and native opening updater, including their exact progress and black-hold edges.

The public name is **Mario Kart 64 3DS Port Complete**. Internal CMake targets, canonical
package filenames, HOME metadata, title ID and SD data paths intentionally keep
their existing names. There is no updater. The preceding fork is
`Epic0522/mario-kart-64-3ds`; the new independent remote URL is not yet assigned.
Local history migration is complete; remote creation/publication is not.
