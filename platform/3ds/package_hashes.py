#!/usr/bin/env python3
"""Write SHA-256 for the canonical full-game packages in a build directory."""
import argparse
import hashlib
import pathlib

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('build', type=pathlib.Path)
p.add_argument('--require-cia', action='store_true')
a = p.parse_args()
root = pathlib.Path(__file__).resolve().parents[2]
version = (root / 'platform/3ds/version.txt').read_text().strip()
stem = 'mk64-3ds-v' + version
names = [stem + '.3dsx']
if a.require_cia:
    names.append(stem + '.cia')
lines = []
for name in names:
    path = a.build / name
    with path.open('rb') as stream:
        header = stream.read(4)
        if name.endswith('.3dsx') and header != b'3DSX':
            raise ValueError('Not a 3DSX: ' + str(path))
        stream.seek(0)
        digest = hashlib.file_digest(stream, 'sha256').hexdigest()
    lines.append(digest + '  ' + name)
manifest = a.build / (stem + '.sha256')
manifest.write_text('\n'.join(lines) + '\n')
print(manifest)
