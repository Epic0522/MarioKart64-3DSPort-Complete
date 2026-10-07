# Mario Kart 64 3DS Port Complete: Change Overview

This project continues EstebanPdN's 3DS port and SpaghettiKart. It retains the game core and working platform components while correcting resource management, rendering adaptations, audio, UI and selected gameplay rules. It is neither a complete engine rewrite nor an N64 emulator.

This document describes the current implementation. [Historical validation receipts](platform/3ds/docs/validation/README.md) record successive experiments; superseded approaches must not be treated as additional current features.

Developer Logging is saved and OFF by default for fresh/legacy configurations.
It gates startup/runtime/installer/fault files without disabling HID, FPS or
explicit Memory dump. Disabling joins the startup writer, drains/closes the
runtime log once and stops further automatic log writes. Manual dumps retain
current snapshots and RAM performance samples, never a stale previous log.
Battle spectators after the human bomb is consumed receive current-survivor
voice/feedback through listener zero; the eliminated human's live voice is muted.
Ordinary driving, menu character confirmation and original result voices remain.

## 1. Resource Management and Stability

- Rework ownership and release order behind progressive kart flickering, disappearing geometry and eventual crashes. Every race load, including same-course retry, clears previous course data, kart animation sources, shared race textures and interpolation history.
- Destroy scene, camera and render references before releasing decoded resources. Submitted texture backing survives until GPU completion; oversized working sets use fenced batches, not hidden opponents or partially presented frames.
- Initialize the missing color-combiner cache-key field, bound CPU cache growth and reset it at course transitions instead of continually accumulating equivalent configurations.
- Retain kart animation LOD and bounded caches without adopting long periods of invisible opponents as a memory-pressure solution.
- Correct scene teardown and camera construction order when entering staff credits after the Special Cup. Avoid building racing collision data that credits immediately discard; preserve the scoped audio-loading path across cuts.

## 2. Old 3DS Performance Regression

The supplied Old 3DS log identified repeated SD reads after full compressed-archive residency failed as the main bottleneck. Modern logic alone did not explain the regression.

- Reduce the Old linear heap reservation from 24 MiB to 20 MiB, returning 4 MiB to the ordinary heap. Expanded New launches retain 24 MiB. The total process allowance and 8 MiB safety reserve remain unchanged.
- When complete residency fails or is reclaimed, use a compressed-entry hot cache bounded to 2 MiB, with bounded metadata and normal decompression/CRC validation.
- Share the budget with kart prefetch rather than allocating another 2 MiB cache. Allow pressure reclamation and prevent allocation-handler reentrancy during cache mutation.
- Add heap-partition, residency and I/O fields to diagnostics to distinguish SD, CPU, GPU and allocator pressure.

Old 80 MiB / New 124 MiB CIA profiles are allocation limits, not measured peaks. The maintainer reported improvement from this fix; no post-fix dump establishes an exact average or minimum FPS.

## 3. Rendering, Timing and Frame Presentation

- Correct N64 depth/culling state adaptation for nearby bridge polygons, flickering trees, water and prematurely disappearing track geometry. Do not merely increase draw distance to conceal state errors.
- Restore native distance-based train models in Kalimari Desert and omit far-distance wheel animation work. Train counts, collision and gameplay remain unchanged.
- Correct kart shadows on the award podium.
- Restore live displays in Luigi Raceway and Wario Stadium at half the current render rate: 30 captures for 60 rendered frames, or 15 for 30.
- Keep simulation at 30 Hz. Adaptive interpolation can present at 60 Hz when workload permits; frame limiter disables intermediate presentations for a 30 FPS cadence. It cannot guarantee 30 FPS in overloaded scenes.
- Use an independent race clock, excluding pause, HOME/sleep and diagnostic dump time. Reset stale timing on retries, random-cup courses and race re-entry.
- Synchronize the Nintendo-logo-to-title white transition across both screens. Keep the lower screen black during staff credits instead of alternating with a blurred menu backdrop.

## 4. Menus and Dual-Screen UI

- Use consistent blurred lower-screen backdrops and a slow leftward conveyor of the 16 GP course minimaps. Reuse race outlines and drawing scale; start with Luigi Raceway centered, Rainbow Road on the left and Moo Moo Farm on the right.
- Increase shared blurred menu-background brightness by approximately 40%, including Options, Developer, DATA and loading artwork. Keep the blur and race/pause/credits treatment unchanged.
- Preserve blue coloring for the two blue maps on the upper screen; use white on the lower screen. Include the minimap conveyor and menu panels in transition fades.
- Use native item artwork at the upper-left in Clean layout and align the timer's margin. Keep the item window hidden between pickups, then apply native slide-in/out motion at each layout's existing settled location, including AUTO.
- Hide rank in Time Trials and upper-screen HUD during title demos. Remove the large lower-screen place indicator in every HUD preset, retaining portrait rank numerals. Apply native finish enlargement and the eight-phase color flash to the upper place indicator, ending before native results-window conversion; lower HUD contents remain visible after finishing.
- Show all eight race portraits in two lower columns, balanced against the minimap. Rear-four move down and clear the front flight path first. Fly the front-four directly to the upper finish row, without a 2x2 intermediate arrangement or mid-flight overlap. Restore the native right-side entry of ranks five through eight, and hand the first four back to native coordinates for the shared exit. Do not mistake the two result-camera windows for two real players or restore the lower racing columns during conversion. Show the original three lap durations and total in the vacated lower-left area; stop upper result HUD before viewport conversion while retaining lower timing results.
- Account for the screen hinge with a hidden travel band instead of joining the two visible screen edges. Draw the finish portrait stack above the enlarged place indicator, retaining race HUD depths and the native all-eight exit.
- Add full-screen 1P VS with seven CPUs and eight-driver ranking, and 1P Battle with three arena bots. Reuse the original mode artwork, arena spawns, balloons, collision and result menus. Keep real player/camera counts at one; use explicit participant counts rather than pretending to run split-screen. Battle bots plan bounded routes on walkable collision triangles, respect floor separation and safe one-way drops, seek physical boxes, and reset inventory/navigation on retry.
- Repair solo VS bomb collision coverage and rank-sensitive finish jingles, voices and result music. Correct Battle collision section filtering, retain autonomous CPU bombs, and observe survivors after the human's one-use bomb is spent without losing pause/quit access. Losing the human does not end the match; native last-survivor victory still applies.
- Smooth Battle routes using wall-tested, floor-supported body-width lookahead instead of chasing tiny triangle portals. Check the kart's heading and momentum near holes, use stable obstruction recovery, and preserve native collision/hit control. Aim single/triple shells at visible opponents before firing; retain native assembly and shot cadence. Use a short Battle finish jingle, tick-30 winner voice and long result track before opening the result menu.
- Index arena floor queries with a fixed 64 KiB spatial grid rather than scanning every floor triangle for every body probe. Stagger objective decisions at 2 Hz, bound steering refreshes and advance inventory once per simulation image. Do not let Star/mushroom boosts bypass momentum-based hole avoidance. Retain supported short platform drops outside Big Donut, and recover a kart already overlapping a ledge instead of permanently braking there. Empty-handed bots prioritize boxes and avoid reserving the same box; prolonged close fights trigger a stable reposition objective.
- Make solo Battle's short jingle, character voice and long result music reflect the human's win or loss. Cancel deferred result handoffs and fade both native sequence players when leaving solo VS/Battle, including exits during the short jingle. Preserve original multiplayer and menu fade behavior.
- Invoke that cleanup from the actual solo VS/Battle result-menu A/Start branch for all four exit choices, not only GP/pause fade helpers. Keep native cumulative win counts but remove the second-human score separator in solo results. Battle bombs pursue survivors to contact instead of backing away under ordinary close-combat spacing; retain supported routes, collision, stuck recovery and one-use impact.
- Use native ceremony fades on the lower screen, including Skip. Staff remains black during its forced state-255 course reloads and stale snapshots, including retained targets and loading artwork. Battle chooses physical boxes by cached route length, retains sensible targets, temporarily rejects pickup routes without progress and excludes other-floor opponents from close-combat retreat. Empty hands prioritize pickup even during Star; armed bots favor usable shots and avoid Star/Boo targets.
- Extend AUTO to solo VS/Battle and keep observed-driver controls, native feedback, pause/results and default gameplay cameras. Battle AUTO and spectators after the human bomb is consumed use only near/far gameplay-follow views, toggled with ABXY; no cinematic orbit or HUD suppression. Manual Battle controls and GP/VS cinematics are unchanged. Turbo works globally, with momentary race C-Stick and edge-triggered ZL toggle, pause/modal guards and scaled, distinct accelerated timing. Battle portraits reorder by balloon count, keeping ties stable and spent-bomb portraits last.
- Clear Turbo's active speed and ZL latch at manual native race initialization, including same-course retry. Continuous AUTO courses/retries retain the toggle; a fresh launch starts off. Do not clear it on Pause/Continue or overwrite the saved multiplier. Countdown rendering passes its first combiner result through the second cycle, avoiding the interpreter's neighbouring texture slot without modifying sprite pixels or the accepted finish flag. Preserve the starter's native bilinear sampling. Dual-cycle layouts keep unused texture coordinates finite instead of dividing by missing texture dimensions. These changes alone did not eliminate the starter speck; see the retessellated submission below.
- Fix arena escape routes overwritten by moving-opponent refresh; maintain matched route triangles/endpoints and discard stale routes after native hits/retrieval. Armed bots approach shots instead of backing away. Use floor/wall-tested short reverse recovery and random distinct Battle opponents. Give all four participants native Lakitu rescue controllers and stop repeated OOB contact from resetting retrieval progress. These host checks do not prove all hardware stuck cases fixed.
- Center continue/quit in the pause-only **Pause** tab without action-row separators; preserve other settings pages. Referee-specific filtering and transparent-fringe rejection replace its unsupported fading-path dither, restoring normal state afterward. Native sprite data stays unchanged; the finish flag is accepted on console.
- Retain the starter's opposite-diagonal submission with its original four vertices, UVs and shadows. Five captured hardware frames have no pure-black pixels at the formerly affected upper-left corner; the maintainer reports the speck returning after A restored the old path. This correction is now unconditional for starters, including demo/camera variants, and independent of Modern Logic. The exact GPU/display-list cause and full animation coverage remain open. Remove the temporary capture UI, per-frame hooks, snapshots and SD writer; normal Logging and Memory dump remain available, and saved evidence is untouched.
- Follow the local arena floor and banked tyre support rather than probing slopes at a fixed Y. Index wall queries, preserve supported platform drops, detect stalled forward progress despite sideways sliding, and lock a verified local escape target against item/pursuit replanning. Keep native collisions, hit penalties and rescue authority. Developer dumps include a bounded `battle-ai.txt` history. Later maintainer feedback accepted the polished build; this does not prove recovery from every possible arena stall. Pause actions use a persistent linear sampler of the native font without allocating another atlas or changing their gradient/layout.
- Fix native EUC-JP lead-byte decoding under ARM's unsigned plain char: VS/Battle score numbers no longer become C/B glyphs. Use supported ASCII dashes for absent Time Trial records, retain valid character names, bound invalid IDs and suppress record panels in other modes. No save conversion or record rewrite is needed.
- Play character-confirmation voices through the original menu-local sound source after returning from racing or skipping results. Keep sample IDs and unity gain unchanged; limit CPU spatial rerouting to racing.
- Prevent custom rank flashing over held black when quitting: apply the native exit opacity to the upper HUD. Close lower shutters before loading and cover the previous menu snapshot on the first race image; do not reuse the previous race's opening fraction or replace the native reveal timing.
- Retain an opaque upper exit hold after every native/custom HUD draw and cover a stale lower race snapshot on the first menu image, including its live timer. After GPU completion, wait until both actual GSP framebuffer presents have been acknowledged before reusing either screen's double buffer; elapsed display counters alone do not prove a newer swap has been consumed.
- Synchronize lower-screen race-entry letterbox shutters to the native viewport-opening fraction. Fade the entire lower screen with the opposed upper results-window exit, retaining black until the next entry; no independent transition timer.
- Move HUD meters into settings; mark unsupported combinations red. Put the settings-save notice on the upper screen.
- Correct the stray results viewport during opposing vertical exit animations and the missing one-pixel right/bottom edges of pause/retry masks.
- Use the native animated tetrahedron pause cursor. Remove obsolete version/update labels and redundant instructions. Title options have Display, Gameplay and Developer; pause retains Game actions.
- Match RANDOM CUP / SELECT AUTO artwork to native pixel lettering, borders and scaling animation. Remove unwanted blue letter edges while retaining rectangular borders. Random-cup captions use ALL RANDOM or DRIVER / LEVEL, with Extra as the mirrored class label.
- Support award-ceremony skipping with a visible hint.

## 5. Controls

- Hold X for rear view; SELECT switches the original camera distance.
- L and Y both use items. Circle Pad up/down selects supported throw directions.
- With modern logic, holding B brakes to a stop and then reverses, without requiring a second press.
- Holding A while selecting Continue carries acceleration through pause recovery.
- Move the checkpoint-based ZR race-completion shortcut into Developer → ZR debug, disabled by default.

## 6. Optional Modern Logic

The switch controls the added CPU/item rules and reverse. OFF retains original gameplay rules; stability, rendering, UI and audio repairs remain enabled. The existing saved setting key remains compatible.

- CPUs collect actual item boxes and wait for roulette instead of receiving free timed items or firing immediately. Item-box route seeking applies in manual and AUTO races; correct early Turnpike steering that pushed cars into barriers.
- Expand the CPU item pool while reducing CPU lightning opportunities to the rear ranks. Human item probabilities remain unchanged. General attack strategy favors leaders, but red shells still target the immediately preceding racer.
- Red/blue shells follow the road before committing to continuous target pursuit, avoiding the approach/return-to-center loop. Travelling blue shells can hit intermediate racers and continue to their main target.
- Warning audio belongs to the threatened player or observed AUTO racer, rather than playing globally whenever any CPU launches a blue shell.
- Modern blue warnings include passing shells behind the listener, not only shells targeting them; targeted red and nearby rear-Star warnings stop when the threat moves ahead. Preserve positional attenuation and original OFF audio behavior.
- Show the nearest eligible rear threat in a white/pink flashing, closed frame at the top of the lower screen. Reuse native red/blue shell and Star icons; follow bearing relative to the player or watched AUTO racer's heading. Slide in/out without changing settled HUD positions, allocating race textures or refreshing the full minimap at presentation rate.
- Match warning left/right placement to the native camera's handedness. Tune only the modern red/blue/Star alarm gain to 70% of its original extended-warning level. Extend red/Star eligibility and audible warning reach by 50%, retaining rear-only checks, positional attenuation and original OFF audio.
- Modern CPUs can pick up, roll and use the next item while a consumed Star remains active; its effect no longer occupies the inventory strategy or freezes held-item timers.
- Apply full CPU item/terrain penalties independent of camera visibility or distance. Preserve native reaction types: green shells tumble; red/blue shells launch.
- Require two seconds of recovered normal driving before another ordinary item hit; lightning is exempt. Protected targets do not keep a pursuing shell attached until protection expires.
- Rotate the complete portrait, border and rank numeral as a rigid square during hit reactions, without a stationary black backing, shrinking or per-slot clipping. Each driver owns a paused-aware rotation phase and finishes the current whole turn after damage ends. Refresh portrait identities and the observed-driver border from the same live ranking before presentation; rank/color changes bypass the ordinary lower-map cache interval. Keep real minimap headings, use native purple dots enlarged by 20% for opponents and white leader flashes, and draw only the human/observed driver's full kart above them. Show opponent reaction text and visible acceleration smoke.
- Replace excessive leader propulsion and abrupt parking with smooth pack-pacing constraints. Do not retain earlier hard-stop or whole-field walking-speed experiments. Preserve catch-up ability and natural gaps from item use, rather than guaranteeing an unchanged tight pack.

## 7. Random Cups and AUTO

- GP Map Select + SELECT: four random tracks using the selected driver and class.
- Game Select + SELECT: random driver and four tracks, with 150cc or Extra chosen independently for each course. Time Trials have no random-cup entry.
- Character Select + SELECT: AUTO uses the cursor's driver; all eight karts are CPU-controlled. The selected driver gets principal-rival-level pacing and owns the cup result regardless of which racer is observed.
- Start in gameplay view. Left/right changes the observed racer; A/B/X/Y toggles gameplay/cinematic view. Keep pause, continue, retry and quit, while locking manual driving/item controls.
- Show the approved AUTO pixel lettering centered in the lower LAP/TIME row, using the native PUSH START BUTTON blink cadence. The center item window does not overlap it in AUTO.
- Apply the same modern item and pack rules as manual racing when enabled. Correct permanently reduced speed after hits, unnecessary corner braking, double consumption of triple mushrooms and unlimited golden mushrooms.
- Restore roulette, item, hit-celebration and finish voices. Suppress repeated overtaking voices for the observed driver and add avoidance for obstacles such as penguins.
- In AUTO, confirm GP/random-cup results, poor-finish Retry and VS/Battle result choices every 0.5 seconds of real time. GP pulses also accelerate native score-counting gates; poor GP finishes select Retry rather than Quit. Pause, fade and not-ready states stop/reset the pulses. Never inject controller buttons into the live race or unrelated menus; manual result input is unchanged.
- Do not write AUTO awards or unlock progress through either native medal entry point. Keep race points intact; only the ending's in-memory character order is swapped to show the selected driver in the original fourth-place no-trophy/bomb-car scene. Confirm its exit only after the native presentation delay.

## 8. Audio and HOME Banner

- Prioritize pickup/hit requests and enlarge bounded pending-request pools while preserving the native playback ABI. Queue capacity is not the number of simultaneous playback voices.
- Attenuate opponent pickup, item and hit audio by position instead of playing distant effects at full volume.
- Finalize the true 3D logo and diagonally waving flag, reduced in scale and facing HOME's camera without its default continuous rotation.
- Keep short title MK64 3DS Port, long title Mario Kart 64, author Epic&EstebanPdN, title ID, approved banner and save location unchanged. The public Complete name does not alter package metadata.

## 9. Code Organization and Release Boundaries

- Keep root CMake focused on options and targets. Split generated adaptations into resources, items, audio/menus, ending, world and renderer modules, retaining replacement-point checks to detect upstream drift.
- Centralize heap/cache capacity policy and document ownership/fence responsibilities. See the [code map](platform/3ds/docs/ARCHITECTURE.md) for entry points and regression routing.
- Remove the updater, its dependencies and unnecessary network permissions. Application upgrades are manual.
- Exclude builds, ROM/O2R, saves, dumps, ELF and private machine files from public source. Retain upstream credits and notices.
- Keep canonical mk64-3ds-vVERSION.cia / .3dsx names and existing installation/data contracts. The current source repository is Epic0522/mario-kart-64-3ds; fork detachment, repository renaming, the root-license decision and publication still require maintainer action.

## Verification Status

E16 was accepted by the maintainer after the final iterative console polish.
This cleanup retains that exact verified CIA/3DSX pair and does not rebuild or
retune runtime behavior. See the [final HUD/AUTO receipt](platform/3ds/docs/validation/LEADER_AUTO_CONFIRM_VALIDATION.md)
for host and package evidence. Acceptance is not an exhaustive performance claim.

Earlier hardware feedback confirmed four-cup completion, followed by further corrections including staff transitions. It does not establish exhaustive coverage of the latest binary. The latest Old cache package has positive practical feedback, but no new quantified performance log.

The latest cache package passed seven ASan/UBSan resource tests, GPU-recycling and combiner regressions, device builds and CIA/3DSX package checks. These validate ownership, content and regression boundaries, not real-console FPS, sound, appearance or long-session behavior.

See the [Old 3DS validation receipt](platform/3ds/docs/validation/OLD3DS_ARCHIVE_CACHE_VALIDATION.md) for package hashes and detailed scope. Future reports should include model, settings, scene, cache state and FPS, not just a single average.
