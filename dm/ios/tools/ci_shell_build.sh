#!/bin/bash
set -euo pipefail
IOS=$(cd "$(dirname "$0")/.." && pwd)
DM=$(cd "$IOS/.." && pwd)
OUT=${1:?Pass a new output directory}
mkdir -p "$OUT"
OUT=$(cd "$OUT" && pwd)
cd "$IOS"
cp tools/shell_templates/dm_hud_exp.h ../host/
cp tools/shell_templates/dm_vault_tab.h ../host/
cp tools/shell_templates/dm_touch_native_art.c Sources/
touch ../host/dm_bright_levels.inc
mkdir -p Gen
python3 tools/gen_build.py ../host Gen > Gen/dm.xcconfig
printf '\nOTHER_CFLAGS = $(inherited) -DDM_SHELL -mcpu=apple-a17\nOTHER_LDFLAGS = $(inherited) $(DM_FRAMEWORKS)\n' >> Gen/dm.xcconfig
printf '// Shell: game images are supplied by the local packer.\n' > Gen/embed.S
for d in assets/NativeHUD/v4/hud_v4_ui assets/NativeHUD/mapgoal romfs_overlay romfs_overlay_variants romfs_overlay_astc romfs_overlay_astc_variants dm msl save_ref save_ref_warlock; do mkdir -p "Gen/$d"; done
export DM_BUNDLE_ID=org.d2rmobile.app DM_TEAM_ID=0000000000
python3 tools/project_free.py project.yml project_free.yml
python3 - <<'PY'
from pathlib import Path
import re
p = Path('project_free.yml')
s = p.read_text()
s, n = re.subn(r'    preBuildScripts:.*?(?=    sources:)', '', s, flags=re.S)
assert n == 1
p.write_text(s)
PY
xcodegen generate --spec project_free.yml
xcodebuild -project DmIOS.xcodeproj -scheme DmIOS -configuration Release \
  -destination 'generic/platform=iOS' -derivedDataPath "$OUT/dd" \
  CODE_SIGNING_ALLOWED=NO CODE_SIGNING_REQUIRED=NO CODE_SIGN_IDENTITY= \
  DM_DEVICE_FAMILY=1,2 build > "$OUT/build.log" 2>&1 || { tail -80 "$OUT/build.log"; exit 1; }
mkdir -p "$OUT/Payload"
cp -R "$OUT/dd/Build/Products/Release-iphoneos/DmIOS.app" "$OUT/Payload/"
APP="$OUT/Payload/DmIOS.app"
V=$(sed -n 's/^#define DMSH_CHAIN "\(.*\)"/\1/p' ../gfx/nvn/dmsh_chain.h)
test -n "$V"
sed "s@^msl/b-[^/]*/\$@msl/b-$V/@" tools/shell_templates/shell_manifest.txt > "$APP/dm/shell_manifest.txt"
printf 'PRODUCT=%s\nCHAIN=%s\nKIND=ios\nSHADER_SRC=58f99c2ccd5d8661\nSPIRV_CROSS=1.4.357.0\n' "$(head -1 ../PRODUCT)" "$V" > "$APP/dm/shell_info.txt"
python3 - "$APP" "$OUT" <<'PY'
import pathlib, plistlib, struct, sys, zipfile
app, out = map(pathlib.Path, sys.argv[1:])
assert plistlib.loads((app/'Info.plist').read_bytes())['MinimumOSVersion'] == '18.4'
b = (app/'DmIOS').read_bytes()
assert struct.unpack_from('<I', b)[0] == 0xfeedfacf
p = 32
versions = []
for _ in range(struct.unpack_from('<I', b, 16)[0]):
    kind, length = struct.unpack_from('<II', b, p)
    if kind == 0x32: versions.append(struct.unpack_from('<I', b, p+12)[0])
    assert length >= 8
    p += length
assert versions == [(18 << 16) | (4 << 8)], versions
files = [f for f in app.rglob('*') if f.is_file()]
assert not any(f.suffix in ('.bin', '.tsv', '.data', '.metallib', '.nso', '.nca', '.keys') for f in files)
assert not any(f.name == 'Guest' for f in files)
with zipfile.ZipFile(out/'D2RMobile-ios18-shell.ipa', 'w', zipfile.ZIP_DEFLATED) as z:
    for f in files: z.write(f, f.relative_to(out))
with zipfile.ZipFile(out/'D2RMobile-ios18-shell.ipa') as z: assert z.testzip() is None
print('Verified iOS 18.4 shell, no game payload,', len(files), 'files')
PY
