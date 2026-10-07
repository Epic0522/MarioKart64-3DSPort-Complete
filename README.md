# Mario Kart 64 3DS Port Complete

<p>
  <img src="https://img.shields.io/badge/platform-Nintendo%203DS-1f6feb?style=for-the-badge" alt="Nintendo 3DS">
  <img src="https://img.shields.io/badge/output-.3dsx%20%7C%20.cia-6f42c1?style=for-the-badge" alt="3DSX and CIA">
</p>

A native Mario Kart 64 port for the Nintendo 3DS family, maintained by Epic. This project continues EstebanPdN's 3DS port, based on SpaghettiKart, with resource-management fixes, rendering corrections and optional gameplay improvements.

This is an unofficial fan project, not affiliated with Nintendo or Harbour Masters. No ROM, O2R archive, save or extracted gameplay resources are included. A compatible USA ROM is required.

## ⬇️ Download and Installation

This project is being prepared as an independent repository; its new publication
URL has not been assigned yet. The preceding fork is
[Epic0522/mario-kart-64-3ds](https://github.com/Epic0522/mario-kart-64-3ds), not a
claim that the Complete source has already been published there. Use **this
project's** CIA or full-game 3DSX, not an upstream package. There is no built-in updater.

1. Install the CIA with FBI, or launch the 3DSX with Homebrew Launcher.
2. Place a big-endian USA ROM at `sd:/3ds/MK64/mk64.z64`.
3. First launch validates and extracts `mk64.o2r`. Keep the console charged; START cancels extraction safely.

Later launches reuse the archive; an existing compatible SpaghettiKart O2R can also be used. Upgrades replace only the application: retain `sd:/3ds/MK64/` to preserve saves, ghosts and settings.

**Complete** is the public project name only. HOME metadata stays **MK64 3DS Port** / **Mario Kart 64** / **Epic&EstebanPdN**; package filenames, the title ID, approved banner and save location are unchanged.

## ⚙️ Features

- **Resource lifetime fixes** — bounded caches, GPU-safe texture recycling and complete course/kart resets on race transitions, including retries.
- **Old 3DS I/O improvements** — revised heap partitioning and a bounded compressed-entry cache reduce repeated SD reads without removing memory-safety protections.
- **Rendering corrections** — depth/culling, transparent trees and water, podium shadows, results viewports and credits transitions.
- **Dual-screen UI** — five HUD layouts, native item graphics, crisp scrolling course minimaps, blurred menu backdrops and original-style RANDOM CUP / AUTO artwork.
- **Audio and feedback** — prioritized effect requests, positional CPU sounds, hit/finish voices and opponent reaction effects.
- **Optional modern logic** — real CPU item-box collection, adjusted CPU item rules and penalties, smoother pack pacing, road-following shells and held-brake reverse.
- **Random cups and AUTO** — four-race random cups and spectator cups driven by the game's CPU logic.
- **Solo VS and Battle** — race against seven CPUs in VS, or face three arena bots in four-kart Battle, with a single full-screen view.
- **Timing and presentation** — an independent race clock, synchronized transitions and a selectable 30 FPS frame limiter.
- **Animated 3D HOME banner** — a front-facing logo and waving flag, without HOME's default continuous rotation.

Modern logic is optional; disabling it restores original gameplay rules, **not** the original crashes or UI bugs. See the [change overview](FIXES.md) and [changelog](CHANGELOG.md) for scope and history.

Select VS or Battle under **1P GAME**. VS uses an eight-driver grid and race rankings, with GP-style finish voices and result music. Battle retains the original arenas, three balloons, item table and last-survivor victory rule; bots use floor-supported, wall-tested routes, prioritize real item boxes when empty-handed and reposition after prolonged close combat. After your one-use bomb is spent, follow surviving bots until the match ends; pause and quit remain available. Arena navigation and basic bot inventory remain available with modern logic disabled, since the original game has no Battle CPU opponents.

The simulation runs at 30 Hz. Adaptive presentation can reach 60 Hz when workload permits; the limiter disables midpoint presentation. Sustained 60 FPS is not guaranteed. Local multiplayer is not available; solo VS/Battle opponents are computer-controlled.

## 🎮 Controls and Configuration

| Control | Manual racing |
| --- | --- |
| Circle Pad | Steer; up/down selects item throw direction |
| A | Accelerate; held A carries through pause → Continue |
| B | Brake; hold through a stop to reverse with modern logic |
| R | Hop / drift |
| L or Y | Use item |
| X | Hold to look behind |
| SELECT | Switch camera distance |
| START | Pause / continue |
| C-Stick | Hold for configured Turbo Speed |
| ZL | Toggle configured Turbo Speed on/off |
| Touch | Lower-screen menus and settings |

L on Game Select opens options. Title options contain **Display**, **Gameplay** and **Developer**; pause also keeps the **Pause** page with centered continue/quit actions. HUD meters are settings, not Y. Turbo Speed fast-forwards game logic globally, including AUTO and menu/ceremony sequences; it is not a kart-speed upgrade. Hold C-Stick during a race or press ZL to toggle. Pause and captured menus suppress acceleration; setting X1 clears the session-only toggle. Manual race entry/retry resets Turbo; a continuous AUTO session retains its toggle across courses and retries. A fresh game launch starts with Turbo off.

| Fresh setting | Default |
| --- | --- |
| HUD layout | Clean |
| Render distance | Low |
| Modern logic | On |
| Frame limiter | Old 3DS: On / New 3DS: Off |
| ZR debug | Off |
| Logging | Off |

Saved settings take precedence. Unsupported HUD meter combinations appear red. Time Trials hide rank; title demos hide the upper HUD. After finishing, the upper HUD retains only the enlarged, flashing place indicator until native results-window conversion begins; lower HUD contents remain visible. Item windows use native pickup/slide motion rather than a permanent empty frame. Lower-screen race entry uses expanding letterbox shutters, and exit fades with the upper results windows. Modern logic changes take effect next race.

## 🎲 Random Cups and AUTO

| Menu | SELECT action |
| --- | --- |
| GP Map Select | Four random tracks with the chosen driver/level |
| Game Select | ALL RANDOM: random driver; 150cc or Extra each course |
| Character Select | AUTO using the cursor's driver |

Time Trial Map Select has no random cup button. Extra is the mirrored course mode. The award ceremony can be skipped with A or START.

AUTO is available in GP, VS and Battle. It hands all eight racing karts, or all four Battle karts, to the CPU. The selected driver owns the result and receives principal-rival-level pacing in races; changing the observed driver does not change that selection. Modern item/pace rules also apply in AUTO when enabled. Gameplay view is the default; Battle offers only near/far gameplay-follow views, not cinematics. Battle standings follow balloon counts with stable ties and eliminated drivers last. Solo Battle draws three distinct random opponents from the seven unselected characters.

Circle Pad / D-pad left or right changes the observed driver. A/B/X/Y switches between gameplay view with HUD and cinematic view without HUD in GP/VS, or near/far gameplay view in Battle. Driving/item inputs are locked; pause, continue, retry and quit remain available. Finish voices refer to the selected driver, not the camera target.

AUTO confirms ready result/retry menus every 0.5 seconds of real time, including GP score-counting fast-forward. It selects Retry after a poor GP finish; VS/Battle use the currently selected native result action. Pause, fades and unfinished menu entry suppress confirmation. The ending plays the original fourth-place no-trophy/bomb-car scene for the selected driver, then returns automatically after its normal presentation. Actual race points are unchanged. AUTO does not record awards or unlock progress in the save; ordinary manual awards still work.

## 🛠️ Diagnostics

Developer → Memory dump exports performance samples, settings and resource data under `sd:/3ds/MK64/dump/`. Dump time is excluded from FPS measurements. Clean dumps removes diagnostic files, not ROM/O2R/saves/settings. Developer → ZR debug is off by default; holding ZR when enabled advances through checkpoints to finish a race.

Developer → **Logging** defaults to **OFF**, including existing configurations.
When enabled, it records startup, runtime, installer and fault logs. The saved
switch takes effect immediately; disabling stops the startup writer and closes
the runtime log. Input, FPS/overlay displays and explicit Memory dump still work
with logging off. A manual dump then contains current snapshots/performance data,
not an old runtime log. Normal save/settings/cache writes are not disabled.

Starter sprites use a corrected quad diagonal while preserving the original art,
UVs and shadows; this repair is independent of Modern Logic. Temporary countdown
capture controls and SD writers have been removed. Developer retains Memory dump,
FPS, overlay, dump cleanup, ZR debug and Logging. Existing captures remain untouched;
local inspection tools and validation records document the investigation;
these maintenance files are not included in the public source snapshot.

CIA allocation limits are 80 MiB on Old models and 124 MiB on New models; these are not measured memory peaks. The maintainer accepted E16 after iterative console testing, including the final minimap/AUTO polish. Earlier feedback confirmed four-cup completion and an improved Old 3DS cache regression. This is not a quantified latest-FPS or exhaustive all-model/all-course/long-session claim. Host tests, package verification and console testing remain separate.

## 🏗️ Building

Initialize pinned submodules recursively, then build:

```sh
git submodule update --init --recursive --depth 1
./platform/3ds/build.sh
```

See [BUILDING.md](BUILDING.md) for dependencies and package verification, the [code map](platform/3ds/docs/ARCHITECTURE.md) for module boundaries, and [RELEASING.md](RELEASING.md) for independent publication checks.

## 📁 Consolidated Workspace

The three former local project directories are consolidated under
**MarioKart64-3DSPort-Complete**. This root is the only maintained, playable
game and release entry point; its approved E16 packages remain unchanged.

| Location | Purpose |
| --- | --- |
| `platform/3ds/` | Maintained runtime, adaptations, artwork and packaging helpers |
| `third_party/SpaghettiKart/` | Pinned production game/engine dependencies |
| `build-3ds/game/` | Latest approved CIA, full-game 3DSX and hash manifest; local/ignored |
| `build-host/retained/` | Compressed diagnostics, package metadata and verification logs; local/ignored |

Local-only experiments, regression suites, preview tools and historical
validation records are ignored by Git and excluded from the public snapshot.
They remain on the maintainer's disk; normal game builds do not require them.
Fresh public clones do not need private archives or logs. The workspace is now
a standalone Git repository, not a linked worktree. Earlier local worktrees,
uncommitted changes and their complete shared histories were verified and
archived outside the project before starting a new source history. Production
dependencies retain their exact upstream commit IDs and source notices; their
local checkouts use self-contained shallow Git stores with no old-workspace
alternates. No remote publication is implied by this local migration.

## 🫡 Credits

- **Epic** — continued 3DS runtime, resources, rendering, audio, UI/input and gameplay fixes.
- **EstebanPdN** — original [Nintendo 3DS port](https://github.com/EstebanPdN/mario-kart-64-3ds).
- **Harbour Masters and contributors** — [SpaghettiKart](https://github.com/HarbourMasters/SpaghettiKart), libultraship and Torch.
- **N64 decompilation and 3DS homebrew communities.**
- **SM64 3DS Port Ultimate** — techniques studied from the [related project](https://github.com/Epic0522/Super-Mario-64-3ds-port---Ultimate).
- **Codex** — assisted implementation and verification.

Source origins and third-party notices are retained. Independent publication does not erase ancestry or replace existing license terms; see [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
