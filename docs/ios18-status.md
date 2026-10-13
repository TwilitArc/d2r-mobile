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

This is not a full application link/build or GPU execution test. No iOS 18 IPA has been built or signed, and no device test has been run. Compiler warnings remain.

## Required next steps

1. Source candidate has been pushed and its Apple SDK compilation check passed. Its output is a source/API compilation check, not an IPA.
2. Run the graphics regression tests on an Apple machine with a usable Metal device, including `t_rt_range` (now using the legacy descriptors and checking explicit pool unregistration).
3. Build a new shell from this source on macOS, using the project's existing `dm/ios/tools/shell_build.sh` process. This full shell build requires generated game inputs; no game files or derived game assets have been uploaded in this task.
4. Package that shell locally on Windows with `dm/tools/d2rm_pack.py`. Reuse the verified `game/` inputs and `iPhone-resources/` beside this source checkout.
5. Sign with the user's chosen tool and test on iOS 18.4.1: clean launch, resource import, shader compilation, character creation, entering a game, combat, changing acts, touch/controller input, save/load and background/resume.

Only passing compilation and device tests can establish that this is a working compatible version. Keep the original iOS 26 IPA separate.
