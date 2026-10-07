#!/usr/bin/env python3
"""Packaging guard: keep private files out and preserve the approved HOME asset."""
import hashlib
import pathlib
import subprocess

root = pathlib.Path(__file__).resolve().parents[2]
private = (
    'experiments/', 'platform/3ds/tests/', 'platform/3ds/docs/validation/',
    'platform/3ds/banner_3d/', 'build-host/', 'build-3ds/', 'build-release-',
    'local-reference/', 'output/', 'test-results/', 'coverage/',
)
forbidden = {'.o2r', '.npak', '.z64', '.n64', '.v64', '.srm', '.sav', '.sra',
             '.fla', '.eep', '.dmp', '.elf', '.cia', '.3dsx', '.pyc', '.log'}
instructions = {'AGENTS.md', 'PROTOCOL.md', 'WORKFLOW.md', 'GITHUB_RELEASE_WORKFLOW.md'}
raw = subprocess.check_output(
    ['git', 'ls-files', '--cached', '--others', '--exclude-standard', '-z'], cwd=root)
for entry in raw.split(b'\0'):
    if not entry:
        continue
    name = entry.decode()
    path = pathlib.PurePosixPath(name)
    if name.startswith(private) or path.suffix.lower() in forbidden or path.name in instructions:
        raise SystemExit(f'Private/generated file in release source: {name}')

banner = root / 'platform/3ds/assets/banner/banner.bnr'
expected = '17f67b7331c956a1e8a99f65f900a67b4ec838576640cdb63b754ad0f66e9e53'
if hashlib.sha256(banner.read_bytes()).hexdigest() != expected:
    raise SystemExit('Approved HOME banner changed; review before packaging.')
build = (root / 'platform/3ds/build.sh').read_text()
if 'assets/banner/banner.bnr' not in build:
    raise SystemExit('Packaging no longer uses the approved HOME banner.')
if 'cp "${BUILD}/mk64-3ds-game-v${VERSION}.3dsx" "${BUILD}/mk64-3ds-v${VERSION}.3dsx"' not in build:
    raise SystemExit('Canonical 3DSX is not copied from the complete game target.')
print('PASS release source boundary, approved HOME banner and full-game package target')
