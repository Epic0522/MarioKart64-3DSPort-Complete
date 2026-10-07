# Approved animated HOME banner

The default CIA uses `banner.bnr` directly. It is the hardware-confirmed
front-facing composition: 75% size, logo in front, group centre between the
logo and flag, animated diagonal checkered flag, no HOME-menu rotation.
Do not silently fall back to `../banner.png` or re-export billboard bones.

Production banner SHA-256:
`17f67b7331c956a1e8a99f65f900a67b4ec838576640cdb63b754ad0f66e9e53`.

`banner.cgfx`, `banner.gltf`, `banner.bin`, texture inputs and `manifest.json`
retain the production model/export inputs. The binary `.bnr` is the exact
approved packaged model/audio, not a ROM or O2R resource archive. Model texture
references are relative. Keep the approval hash when reorganizing these files.

An explicit alternate `MK64_3DS_BANNER_CGFX` requires bannertool and packages
that model with `MK64_3DS_BANNER_AUDIO` (default `../banner.wav`). Such an export
is a new visual result, not implicitly hardware-approved.
