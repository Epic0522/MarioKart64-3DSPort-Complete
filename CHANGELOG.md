# Changelog

## Mario Kart 64 3DS Port Complete (unreleased)

Current maintained implementation; see [FIXES.md](FIXES.md) for the detailed
overview and [README.md](README.md) for usage.

- Start an independent local source history in the same consolidated directory.
  Archive and verify the complete preceding workspace outside the project;
  remove linked-worktree/shared-history dependencies and retain pinned upstream
  sources with shallow, self-contained Git stores. No runtime behavior or
  accepted package changes, and no remote publication.
- Consolidate the three local workspaces under MarioKart64-3DSPort-Complete.
  Keep the maintained game at the root, preserve independent native research
  under experiments/native, and retain the old dirty worktree/full shared Git
  history as ignored local reference. Repair worktree/submodule links, document
  separate build boundaries and verify a clean game build plus both test suites.
- Adopt the Complete public project name and current Epic0522 repository link.
  Preserve the accepted E16 packages, HOME metadata, title ID and save paths.
  Consolidate maintenance-test entry points and remove obsolete local build
  products without changing runtime behavior. Fork detachment/renaming remain
  maintainer actions, not changes performed by this source cleanup.
- Flash the human/observed leader's full minimap kart white as well as CPU dots.
  Confirm AUTO results/retry every 0.5 seconds of real time, including native
  GP score fast-forward; keep pause/fade guards and original transition routes.
- Enlarge native minimap dots by 20% without moving their centers. Refresh
  standings after simulation and invalidate the lower cache on rank/portrait/
  border changes. Track damage rotation per driver and finish whole turns.
- Show all eight racers in two lower-screen portrait columns. Rear-four exit
  downwards before the front-four fan/hinge flight; retain native all-eight
  upper result entry/exit and lower lap breakdowns. Remove the large lower
  place indicator in every HUD preset.
- Display the approved AUTO lettering with the native PUSH START BUTTON blink
  cadence. Use native purple minimap dots for opponents, flash the leader white,
  and keep only the human/observed driver as a full kart. Move damage rotation
  from the map into a rigid portrait/border/rank group without a fixed backing.
- Add Developer Logging, saved and OFF by default for fresh/legacy settings.
  Gate startup/runtime/installer/fault log writes while preserving input, FPS,
  overlays and explicit diagnostic dumps. Apply ON/OFF immediately.
- Route spent-bomb Battle spectator feedback to the currently watched survivor
  and silence the eliminated human's live voice; retain original result voices.
- Automatically advance AUTO GP/random-cup native results and select Retry after
  poor finishes. Play the fourth-place ending without changing race points, then
  confirm its native exit. Suppress AUTO award/progress writes and preserve Turbo
  across continuous AUTO courses/retries; fresh launches remain unlatched.
- Extend AUTO to solo VS and Battle, including four-kart observation, arena
  cameras, native finish audio and pause/results input. Make Turbo global, keep
  race C-Stick hold and add a session-only ZL toggle; scale the race clock and
  spread accelerated timestamps across simulation ticks. Clear active Turbo and
  its ZL latch at manual native race initialization, including same-course retry;
  continuous AUTO retains it. Preserve the multiplier and Pause/Continue behavior.
- Keep Battle AUTO and post-bomb spectators in gameplay-follow cameras. ABXY
  toggles near/far follow distance without hiding HUD/audio; left/right changes
  the observed survivor. Manual Battle driving and racing cinematics stay intact.
- Rank Battle portraits by balloons with stable ties. Randomize distinct solo
  opponents. Separate escape endpoints from moving pursuit targets, refresh
  pursuit triangles, replan after hits/retrieval and use supported short reverse
  recovery. Armed bots close in rather than repeatedly backing away.
- Give all four solo Battle participants native rescue controllers and prevent
  persistent OOB contact from restarting their rescue progress every tick.
- Follow banked arena ground in lookahead probes, retain safe platform-drop body
  clearance, and lock local slope/wall recovery against pursuit retargeting.
  Export bounded per-kart control history in Developer dumps; console recovery
  remains a test requirement. Filter pause-action glyphs without changing their
  native artwork, gradient or layout.
- Rename the pause-only Game tab to Pause and center its two actions without
  setting-row separators. Keep other option pages and touch alignment intact.
- Restrict referee sprite filtering/alpha state explicitly on 3DS, without
  changing native art. Keep countdown's second combiner cycle as a passthrough
  instead of importing an unrelated neighbouring tile. Restore native bilinear
  starter sampling and keep unused dual-cycle texture coordinates finite when
  no dimensions were imported. Those changes alone did not eliminate countdown
  specks; the later retessellated submission is described below. The finish path
  is accepted.
- Retain the starter quad's opposite-diagonal submission outside diagnostics.
  The latest five-frame hardware capture has no upper-left corner specks, and
  the maintainer reports them returning when A restored the old submission.
  Modern Logic does not select geometry. Four vertices,
  UVs and native shadows remain intact; finish/rescue sprites keep their path.
  The precise GPU/display-list cause and full animation coverage remain open.
- Remove temporary Countdown capture: Developer row, per-frame input/render
  hooks, framebuffer fences/readback, GPU/native snapshots, SD writer and
  capture-only source-copy API. Keep the starter rendering change unconditional,
  manual Memory dump and Logging intact, and existing SD evidence untouched.
- Extend startup timing back to the first statement in main, covering diagnostic
  setup before the loader. This does not measure HOME-to-main launch time.
- Repair resource ownership, GPU-fenced texture reuse, bounded combiner caching
  and course/kart resets across races, retries and credits.
- Fix Old 3DS cache-residency regression with a 20 MiB linear heap partition and
  shared 2 MiB compressed-entry fallback cache; keep allocation-pressure safety.
- Correct depth/culling, tree/water rendering, distant train work and podium
  shadows. Restore Luigi Raceway and Wario Stadium's half-rate live jumbotrons.
- Add independent race timing and optional 30 FPS presentation; fix stale race
  clocks, synchronized title transitions and credits lower-screen backgrounds.
- Polish five HUD layouts, native item display, menu minimap animation/fades,
  results exit viewports, pause/retry masks and settings presentation.
- Restore pickup-only native item-window slide motion and finish-place scaling/
  flashing without changing settled HUD rectangles. Include independent AUTO
  presentation states; retain demo/cinematic/Time Trial suppression. End the
  upper finish indicator before results-window conversion; retain lower HUD.
- Match lower-screen entry letterbox shutters and exit fades to native viewport
  progress, including the final black hold between races.
- Correct finish portrait depth above the enlarged rank and include a hidden
  hinge interval in the lower-to-upper flight, without changing native exit.
- Add single-player VS (one human and seven CPUs) and Battle (one human and
  three arena bots), with full-screen controls, native results and real boxes.
  Build bounded, height-aware arena navigation from the original collision mesh.
- Correct arena section-ID filtering that prevented bot navigation. Keep CPU
  bombs autonomous and continue spectator/pause control after the human bomb
  is spent, until the native last-survivor result. Restore solo VS bomb hits
  for all eight drivers and the complete native GP-style finish audio sequence.
- Replace short arena waypoint chasing with body-width, floor-supported steering
  and wall checks. Retain shells until a visible opponent is in the firing cone;
  preserve native triple-shell assembly and firing cadence. Restore solo Battle's
  short finish jingle, delayed winner voice and long result music.
- Keep character-confirmation voices menu-local after returning from a race,
  including skipped results; do not inherit stale CPU spatial attenuation.
- Fade custom upper HUD with native race exit, including its held-black frames.
  Close lower shutters before course loading and cover the stale menu snapshot
  on the first race image, without changing native opening cadence.
- Index arena floor queries in a fixed 16x16 grid, stagger 2 Hz objectives,
  bound steering refreshes and advance inventory once per simulation image.
  Probe momentum even during Star/mushroom boosts; recover safely from ledges
  and retain verified short drops on Double Deck. Reserve boxes and prioritize
  empty-hand collection; reposition rather than indefinitely orbiting opponents.
- Play solo Battle's result jingle, voice and music relative to the human's
  win/loss. Cancel queued sequence handoffs and fade both players when leaving
  solo VS/Battle, preventing result music from restarting after black.
- Wire the actual VS/Battle result-menu A/Start path for Retry, Course Change,
  Driver Change and Quit to that audio cleanup. Retain native cumulative win
  counts and omit the unused second-human score separator in solo results.
- Keep eliminated Battle bombs pursuing surviving humans/CPUs to contact;
  exclude them from ordinary combat spacing and stale roulette cooldowns,
  while retaining floor/wall safety, obstruction recovery and one-use impact.
- Match lower-screen ceremony Skip/normal exit fades to the native timer.
  Keep Staff's lower screen black through native 255-to-9 course reloads,
  stale snapshots, retained targets and enabled loading artwork.
- Prioritize Battle box pickup using cached route lengths rather than distance
  through stacked floors. Keep sensible box/attack targets stable, reject
  stalled pickup objectives temporarily, and do not retreat from opponents on
  another floor. Retain physical boxes, bounded planning and collision rules.
- Correct native EUC-JP decoding for ARM unsigned char, restoring VS/Battle
  score digits. Use ASCII no-record dashes, bound record character IDs and
  suppress Time Trial record panels outside Time Trials; do not rewrite saves.
- Wait for actual GSP presents after the GPU queue fence before reusing either
  screen buffer. Retain final black covers over the upper HUD and stale lower
  race snapshots so live timer text cannot flash during exit.
- Add X rear view, SELECT camera distance, L/Y items, pause-throttle continuity
  and optional held-brake reverse. Move ZR finish testing to a default-off setting.
- Add optional modern CPU items, real box collection, full hit/terrain penalties,
  smoother pack pacing, road-following shells and recovery-based hit protection.
- Add four-race random cups and AUTO spectator cups with fixed selected-driver
  result ownership, view switching, complete feedback and CPU corrections.
- Prioritize bounded sound requests while retaining the native playback ABI;
  restore positional feedback, hit/finish voices and opponent effects.
- Finalize the front-facing animated 3D HOME banner. Keep HOME titles/author,
  title ID, package naming and save path unchanged; the former -Fixed project
  name has since been superseded by Complete.
- Split build adaptations by responsibility, document module/test boundaries,
  retain third-party notices, and remove the updater and its dependencies.

Hardware reports include four completed cups on an earlier test build and
improvement from the latest Old 3DS cache correction. They do not quantify the
latest FPS or establish exhaustive long-session coverage. Host/package evidence
is recorded separately under `platform/3ds/docs/validation/`.

## Historical development entries

The entries below describe earlier builds, not an additive list of current
features. In particular, the updater has been removed and later UI/gameplay
corrections supersede earlier experiments.

## E13 resource-runtime test (unreleased)

- Restore the original near/medium/far train models in Kalimari Desert, without changing train counts, physics, collision, visibility or racer LOD. Keep distant train bodies but skip their original far-distance animated-wheel work.
- Reconnect ceremony shadow indices to the C++ podium's actual object slots and suppress the duplicate ground shadow once karts mount the podium.
- Add cached, strongly blurred color backdrops and a slow right-to-left course-minimap loop to otherwise empty lower-screen menus. The current loop uses the 16 GP courses. Keep race HUD, interactive panels, demo races and ceremonies unchanged.
- Match conveyor minimaps to each course's race-HUD scale (the same 149x184 fitting area), rather than the original tiny 84x52 menu fit. Widen spacing to 192 pixels, retaining 16 pixels/second motion and the existing fixed-size atlas.
- Bound the new menu GPU storage to one 512 KiB map atlas plus a 4 KiB backdrop; perform decoding/blur only on menu asset changes, not during races.
- Initialize all three Fast3D color-combiner comparison-key fields; upstream left `shader_id` uninitialized, allowing identical GPU configurations to accumulate separate CPU cache entries.
- Bound the CPU combiner cache to 256 entries, invalidate saved material pointers on limit resets, and clear it at race transitions without deleting GPU shaders or hiding racers.
- Add thread-safe combiner counts/reset counts to memory diagnostics and a generated-production-code sanitizer regression test.
- Reset stock course resources at every race load, including same-course restarts.
- Destroy scene, camera, track and GPU references before releasing decoded data.
- Clear kart animation sources, shared race textures and interpolation history at that boundary; retain audio and menu resources.
- Fence the GPU before releasing texture backing, rather than waiting only for VBlank.
- Finish oversized texture working sets in fenced batches without presenting partial images or clearing the in-progress color/depth buffers.
- Resolve resident ZIP addresses after allocations that can reclaim the archive mirror.
- Persist pre-recovery renderer faults separately from ordinary buffered logs; distinguish cache limits, linear texture allocation and ordinary-heap failure.

Host sanitizer tests and 3DS builds do not establish physical-console stability.

Hardware feedback (2026-09-30): the previous `combiner-cache-fix` package completed the first cup through the awards ceremony. Later cups remain untested. The newer `desert-menu-fix` package still requires hardware checks for desert performance, podium shadows and menu appearance.

## v1.6-E7

Changes since v1.5.

- Default to 1.0 render scale on both Old and New 3DS.
- Clean the actual HUD buffer allocations instead of the gaps between them.
- Move texture scaling and tile clamping to the GPU vertex shader.
- Service HOME before starting game logic and audio rendering work.
- Stop the audio producer and release its shared-core reservation during suspension.
- Restart audio and reset presentation timing after returning from HOME or sleep.
- Suspend diagnostic input polling while the application is inactive.
- Bound shutdown waits and prevent live worker stacks from being freed on timeout.
- Record startup stages, shared-core reservations, vertex packing and HUD cache costs.
- Fix Old 3DS startup aborts caused by mandatory archive RAM residency.
- Keep optional interpolation storage out of the ordinary archive heap.
- Load compressed resources in small blocks to tolerate heap fragmentation.
- Distinguish memory limits from archive read errors in startup diagnostics.
- Show a small animated Lakitu flag on black screens while the game starts.
- Cache the 32 loading frames locally and release their memory before gameplay.
- Show instantaneous FPS from consecutive presented images in the corner counter.
- Add a touch-adjustable volume slider matching the render slider.
- Allow adaptive interpolation on Old and New 3DS in both supported resolutions.
- Skip interpolation recording when CPU or GPU work exceeds its budget.
- Batch O2R startup reads while retaining compact RAM residency and CRC checks.
- Avoid repeated black-screen redraws during course loading.
- Combine initial EEPROM repairs into one atomic save and skip unchanged writes.
- Refill audio throughout long rendering work, with a barrier before game logic resumes.
- Restore early CPU face rejection before texture processing and vertex submission.
- Avoid renderer state changes for rejected triangles and cache copies for small vertex loads.
- Add eligible GPU vertex transforms and directional lighting with CPU compatibility paths.
- Add bounded matrix and vertex-depth reuse, faster command dispatch, and diagnostic counters.
- Fix the red lower screen during the Nintendo logo and synchronize the white title transition.
- Remove menu labels and race HUD text behind the Update changelog.
- Fix updater certificates, offline notes, flicker, backgrounds, and page navigation.
- Add Update below Close Options with the native Mario Kart menu font.
- Add Stable and Experimental channels with verified CIA/3DSX updates and installation confirmation.

The game simulation remains at 30 Hz; adaptive presentation can reach the 60 Hz display limit when workload permits. E7 performance and compatibility still require physical-console validation.

## v1.5

- Fixed startup, shutdown, and gameplay crashes
- Fixed the graphics command-buffer crash on race results
- Improved Old and New 3DS performance and memory use
- Fixed frame interpolation and frame pacing
- Preloaded game resources into RAM to remove in-race resource reads from SD
- Improved rendering, textures, colors, and audio processing
- Faster on-device game-data extraction
- Always show ROM extraction progress independently of game loading screens
- Added adjustable render scale, render distance, and display filters
- Added five HUD layouts and improved race HUD presentation
- Redesigned Data and Course Data across both screens
- Centered Grand Prix and Time Trial results with text shadows
- Fixed record display and ghost save/load persistence
- Added Race Ghost and Erase Ghost actions in Course Data
- Expanded diagnostic dumps with numbering, RAM capture, and Clean dumps
- Updated the HOME Menu banner and sound
