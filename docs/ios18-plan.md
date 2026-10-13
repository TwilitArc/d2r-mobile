# iOS 18.4.1 Metal renderer implementation plan

Goal: run the iPhone 15 Pro Max build on iOS 18.4.1 after macOS compilation and device verification.

Architecture: a dedicated legacy-Metal source checkout; the upstream Metal 4 checkout and existing IPA remain available. Reuse the original game program, generated data and MSL. Replace command submission and resource binding with the existing Metal APIs.

Constraints: minimum iOS 18.4; no uploads of game files; no claim of runtime compatibility before device testing; no changes to game rules. This Windows host cannot compile or execute the iOS host.

- [ ] Add regression checks for buffer address resolution, inherited shader bindings, indexed drawing, upload compute/blit separation, event ordering and deployment version.
- [ ] Replace Metal 4 initialization and command allocators with fresh legacy command buffers on the existing queue.
- [ ] Bind buffers with concrete MTLBuffer objects and offsets; declare indirect resources for tracked hazards.
- [ ] Split upload compute and blit encoders; migrate GPU texture conversion, special compute passes and presentation.
- [ ] Remove new API dependencies from executable paths, lower App and Guest deployment targets, and use A17-compatible compiler settings.
- [ ] Prepare a macOS source-only compile check, verify source invariants and document unverified runtime requirements.
- [ ] Build a new iOS shell on macOS, package it using the already verified game data, and verify installation, shaders, gameplay and saves on iOS 18.4.1.

The last step requires a macOS build environment and the user's phone. An upstream iOS 26 shell cannot be used to test the new renderer.
