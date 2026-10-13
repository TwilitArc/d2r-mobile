# iOS 18.4.1 port: source candidate, not a verified release

Target: iPhone 15 Pro Max running iOS 18.4.1. App and Guest deployment floor: iOS 18.4.
Base: upstream v1.0.1, ea86e79688f9339319de3e2c52c69b8b13e30e9e.
Product identity: `3.3.94012-ios18-r40`.

## Implemented in this source candidate

- Standard Metal command buffers replace Metal 4 queues/allocators. Existing in-flight limits, upload retries and completion effects remain.
- Shader buffers use registered GPU address ranges to find their MTLBuffer object and offset. Range checks prevent overflow and out-of-bounds indexed draws.
- Argument buffers record only the referenced resources and their read/write usage. The cache uses weak keys and a weak encoder reference. Temporary strong references protect concurrent sampler replacement; command completion retains indirect samplers. Explicit pool removal unregisters old buffers.
- GPU texture uploads end the compute encoder before blit copying and use a separate compute encoder for BC decompression. The existing legacy GPU deswizzle function is reused.
- Bloom, brightness, ASTC conversion and presentation bind concrete textures, samplers and buffers. Presentation is scheduled through the command buffer before commit.
- New managed BackgroundAssets APIs/download extension are removed. Resources are imported into the app folder; the existing direct-download fallback remains available when configured.
- App/Guest deployment metadata is lowered to 18.4 and the host CPU setting targets A17. A separate product identifier prevents accidentally using the original iOS 26 shell.
- Previously validated Windows packaging hard-link fixes are included.

## Verification performed on Windows

- `t_gpu_range.c`: compiled with Clang, `-Wall -Wextra -Werror`; all seven boundary assertions passed.
- `test_ios18_artifacts.py`: three tests passed. The Guest test generates a real fixture Mach-O/framework and checks its minimum version and Info.plist. The remaining checks cover source dependencies and project configuration.
- Free-account project generation succeeded with the revised project structure.
- Using the original iOS 26 shell with this source was rejected before packaging, as intended.
- Git whitespace checks and the macOS check script's Bash syntax check passed.
- Independent source review identified resource-retention/performance and sampler-race issues; those were corrected and re-reviewed.

## Apple SDK verification on GitHub Actions

- Fork: `TwilitArc/d2r-mobile`, branch `ios18-support`.
- Commit `b417d94b28cb9270b2359e6a234c5b4683fe9c42`: macOS compilation check passed on 2026-10-13.
- The runner used Xcode 16.4 / iPhoneOS 18.5 SDK with deployment target 18.4. Syntax/API compilation passed for `nvn_mtl.m` (including `nvn_rtex.inc`), `nvn_deswizzle_gpu.m`, and the modified iOS `main.m`. C boundary assertions and all three artifact tests passed.
- Run: https://github.com/TwilitArc/d2r-mobile/actions/runs/38348785281
- The first compile exposed an omitted Metal 4 dependency in the included extrapolation renderer. It was converted to legacy texture/buffer binding, and the regression check now scans all renderer `.inc` files.

## Full shell build and local packaging

- Full Xcode Release application compilation and linking passed on macOS, targeting arm64 / iOS 18.4: https://github.com/TwilitArc/d2r-mobile/actions/runs/38349441566
- `ci_shell_build.sh` builds from empty table templates without uploading game files. Shell brightness metadata is now loaded from `BRIGHT` records in `derived.tsv`, generated locally by the packer. Two parser boundary tests passed.
- The manifest shader directory is generated from the same chain identifier as the executable. The shell excludes every resource listed for local replacement, including `native_art_manifest.json`.
- The first local overlay run hit a Windows sandbox hard-link denial. Re-running the same authorized packer outside the sandbox succeeded.
- Final local IPA: `D2RMobile-iOS18.4-iPhone15ProMax.ipa`, 122492249 bytes, SHA-256 `d71cb5f1960d51bf9c1123cc96da32b099b3e8ab874967a3ad9c7805ac12c1b4`.
- App and Guest framework Mach-O and plists declare minimum iOS 18.4. ZIP CRC passed, 14335 unique entries, all 26 required items present, 6927 shader reflections share the executable's chain. Brightness data contains 138 records. Original ExeFS hashes passed.
- A duplicate resource manifest from the initial shell was removed while preserving the locally generated manifest and binary contents. The CI shell source was fixed for future builds.

This unsigned IPA has not been signed, installed or run on a physical iPhone. Compiler warnings remain. Successful builds do not establish GPU/runtime correctness.

## Required next steps

### Device feedback and build 2

Two iOS 18.4.1 boot attempts reached resource validation, Guest loading, `nnMain` and Metal initialization, then stopped at `FATAL` because `dm/shader_map.tsv` was missing. The CI shell omitted the two tables copied by the original `run.sh` workflow. Both `shader_map.tsv` and `pso_manifest.tsv` are now copied into the shell; packing rejects shells missing either table. Regression fixtures cover both missing-table cases and an accepted complete shell.

Local r2 IPA: `D2RMobile-iOS18.4-iPhone15ProMax-r2.ipa`, build 2, 122731652 bytes, SHA-256 `eecedcb2548d4dc4af94764cf60b5317cc0122004521e4a49b0b397789c2826a`. Both tables are present; ZIP CRC and all prior package checks passed, with 14337 unique entries. App/Guest executable hashes are unchanged. On 2026-10-13 the user confirmed that this r2 package runs normally on their iPhone 15 Pro Max running iOS 18.4.1. This is a user-reported device result; the full scenario checklist below has not been independently verified.

1. Source candidate has been pushed and its Apple SDK compilation check passed. Its output is a source/API compilation check, not an IPA.
2. Run the graphics regression tests on an Apple machine with a usable Metal device, including `t_rt_range` (now using the legacy descriptors and checking explicit pool unregistration).
3. The new shell has been built using `dm/ios/tools/ci_shell_build.sh`, retaining the original shell loading and runtime table mechanism. No game payload was uploaded.
4. Local Windows packaging is complete. Keep the verified `iPhone-resources/` beside the IPA for manual transfer.
5. Sign with the user's chosen tool and test on iOS 18.4.1: clean launch, resource import, shader compilation, character creation, entering a game, combat, changing acts, touch/controller input, save/load and background/resume.

Only passing compilation and device tests can establish that this is a working compatible version. Keep the original iOS 26 IPA separate.
