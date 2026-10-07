# Building Mario Kart 64 3DS Port Complete

## Dependencies and outputs

Use devkitPro devkitARM/libctru/citro2d/citro3d/3DS utilities, CMake/make, Python 3.11+,
Git and patch. Initialize pinned submodules recursively, not arbitrary latest
versions. No updater-specific network libraries are required.

```sh
git submodule update --init --recursive --depth 1
./platform/3ds/build.sh
```

The script applies/verifies the complete checked-in Torch patch; incompatible
dirty dependencies fail.
Canonical outputs are `build-3ds/game/mk64-3ds-vVERSION.{cia,3dsx}`, with VERSION
from `platform/3ds/version.txt`, plus a matching `.sha256` manifest. The canonical
3DSX is the full game, not a probe.
CIA requires makerom. Set `MK64_3DS_REQUIRE_CIA=1` to fail rather than produce
only 3DSX when makerom is absent. The approved animated banner is included;
bannertool is needed only for an explicit custom CGFX export.

Environment overrides: `DEVKITPRO`, absolute `MK64_3DS_BUILD_DIR`,
`MK64_3DS_JOBS`, `MAKEROM`, and `MK64_3DS_REQUIRE_CIA`.
Additional CMake arguments can be passed to the build script, including
`FETCHCONTENT_SOURCE_DIR_*` overrides for local pinned source caches.

## Module layout

Use the consolidated workspace root, `MarioKart64-3DSPort-Complete`, as the
working directory. Production sources live at the root/`platform/3ds/`.
Local regression suites, preview tools, validation records and experimental
research are excluded from the public repository and are not build dependencies.
The root has its own `.git/` directory; it no longer uses a linked worktree or
the preceding `local-reference/` store. Earlier source/history is archived
outside this project. A fresh clone and production build must not need it.

Root CMake declares options/targets and includes modules in the original order.
`MK64_3DS_SOURCE_ROOT` anchors paths independently of the current module.

- `game_core_3ds.cmake`: core target; `racing_resources`, `racing_objects_items`,
  `racing_audio_menu`, `racing_ending` group generated native adaptations.
- `game_engine_3ds.cmake`: engine target; `world_lifetime`, `world_effects`,
  `world_tracks_bridge` group scene owners and course integrations.
- `renderer_3ds.cmake`: targets; `interpreter_patches` retains ordered Fast3D
  adaptations and specialized geometry/texture/combiner modules.
- `source/`, `include/`: runtime and scoped policies.
- `patches/`: upstream adaptations including Torch extraction.
- `assets/banner/`: exact approved HOME asset and model/export inputs.
- `check_release_source.py`: public-source and approved-package boundary guard.
- `probe_stubs/` and diagnostic source targets: retained because CMake references
  them; do not remove them using broad filename-based test exclusions.

See [the code map](platform/3ds/docs/ARCHITECTURE.md) for runtime ownership,
adaptation entry points and local regression-test routing. The public project name
does not rename internal targets, package filenames, HOME metadata or SD paths.

Generated sources belong in build directories; do not hand-edit them. ROM/O2R,
saves, dumps, build directories and Python caches are local ignored data.

## Source and package verification

The published build workflow uses the packaging guard, not local test suites:

```sh
python3 platform/3ds/check_release_source.py
```

It rejects private/generated files in the source snapshot and checks the
approved HOME banner and canonical full-game target. Package hash verification
is separate from physical-console rendering, memory and performance checks.
The maintainer's regression suites remain local and are not bundled with source.

## Clean verification without replacing the approved packages

Use a separate, disposable directory for a clean build. Do not overwrite an
accepted release merely to check relocation or toolchain compatibility:

```sh
MK64_3DS_REQUIRE_CIA=1 MK64_3DS_BUILD_DIR="$PWD/build-3ds-verify/game" ./platform/3ds/build.sh
python3 platform/3ds/check_release_source.py
(cd build-3ds-verify/game && shasum -a 256 -c mk64-3ds-v1.6-E16.sha256)
```

Use the actual version from `platform/3ds/version.txt` rather than the E16 example
after a later release. Compare HOME metadata and banner with the accepted baseline;
reverse-extract CIA and inspect content hashes separately from homebrew signature
warnings. A rebuilt package is verification output, not automatically the approved
hardware-tested release. Keep the canonical accepted pair until replacement acceptance.
