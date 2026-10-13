#!/bin/bash
set -euo pipefail
HERE=$(cd "$(dirname "$0")" && pwd)
DM=$(cd "$HERE/../.." && pwd)
SDK=$(xcrun --sdk iphoneos --show-sdk-path)

# Source-only compilation: no game files, credentials or signing required.
for src in "$DM/gfx/nvn/nvn_mtl.m" "$DM/gfx/tex/nvn_deswizzle_gpu.m" "$DM/ios/Sources/main.m"; do
  xcrun --sdk iphoneos clang -target arm64-apple-ios18.4 -isysroot "$SDK" \
    -fobjc-arc -fsyntax-only -Wall -Werror=unguarded-availability \
    -I"$DM/gfx/nvn/include" -I"$DM/host" "$src"
done
OUT=$(mktemp -d)
trap 'rm -rf "$OUT"' EXIT
clang -Wall -Wextra -Werror "$HERE/t_gpu_range.c" -o "$OUT/t_gpu_range"
"$OUT/t_gpu_range"
python3 "$HERE/test_ios18_artifacts.py"
python3 "$HERE/test_shell_brightness.py"
