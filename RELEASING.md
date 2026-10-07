# Mario Kart 64 3DS Port Complete — independent release checklist

This checklist does not authorize changing remotes, detaching a fork, creating
a repository, committing/pushing, uploading releases or installing a console title.

1. The local workspace has been migrated to an independent source history.
   Create an empty new remote repository when the maintainer is ready; its URL
   is not yet assigned. Then update download/source links, add that exact remote
   and explicitly authorize publication. The preceding fork is
   [Epic0522/mario-kart-64-3ds](https://github.com/Epic0522/mario-kart-64-3ds);
   do not push the independent history over it or claim publication is complete.
   There is no built-in updater.
2. Review retained notices and source origins. There is no blanket root project
   license declaration; resolve that maintainer decision before publication
   without relabeling third-party code/assets.
3. Include maintained game source/modules, packaging helpers and approved banner inputs. Exclude
   builds, ROM/O2R, saves/dumps, extracted gameplay data and local instructions.
   Exclude `local-reference/` and `build-host/retained/`; they preserve private local
   history/evidence, not release data. Tests, validation receipts, banner authoring/
   preview tools and `experiments/` are local-only and excluded from the source
   snapshot. Run `python3 platform/3ds/check_release_source.py` before publication.
4. Validate from fresh pinned dependency checkouts using the checked Torch
   patch. No unrecorded dirty dependency source may be required. Record tools,
   revisions and results.
5. Build both complete packages with `MK64_3DS_REQUIRE_CIA=1`. Canonical release
   asset names are `mk64-3ds-vVERSION.{cia,3dsx}`; never substitute a probe.
6. Run the maintainer's local regressions and reverse-unpack CIA. Check integrity, metadata, title
   ID and exact approved banner; distinguish homebrew signature warnings from
   content hash failures. Record final SHA-256 and hardware status separately.
7. Preserve the latest verified pair until its replacement passes. Keep source,
   logs/receipts, then remove obsolete generated ROM/ELF using inspected exact
   manifests. Never delete original ROM/O2R/saves.
8. Publish only after maintainer approval of repository/version/notices/testing.
   Supply the package SHA-256 manifest. Do not infer sustained
   60 FPS or all-hardware compatibility from build/host results.

Upgrades are manual CIA/3DSX replacements. Preserve title ID and the data
directory; do not bundle or delete existing saves/settings.

`Complete` belongs to the public project name, not HOME metadata. Keep the short
title `MK64 3DS Port`, long title `Mario Kart 64`, author `Epic&EstebanPdN`, title
ID `0004000005a27000`, approved banner and `sd:/3ds/MK64/` data contract intact.
