---
id: task-055-01-first-hit-vignette
description: "The red damage vignette shows on the very first hit, including in a packaged build."
parent: task-055-first-hit-vignette
kind: task
status: in-progress
---
# First-hit vignette

- goal: The red damage vignette shows on the very first hit, including in a packaged build.
- agent: cpp
- depends_on: none
- inputs: sys-hud-pressure (vault/reference/tech/systems/sys-hud-pressure.md), wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-cheats (vault/reference/tech/systems/sys-cheats.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: find why the first hit shows no vignette (suspects: M_HUDVignette shader compiled on first use, DamageVignette opacity or brush not initialised before the first OnPlayerDamaged, widget not yet ticking); fix at the cause in C++ (e.g. prewarm the material or initialise the widget state at HUD construct); if the fix needs a Blueprint change, list it precisely for a bp subtask; verify in a packaged Development build (BuildCookRun as in build-toolchain): take a shot right after the first hit (rr.Debug.DamageMiner) and describe it; update only existing tests this breaks; docs sys-hud-pressure.
- done_when: build exits 0 with the editor closed; packaged build shows the vignette on the first hit (described shot); all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 70000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-055-first-hit-vignette]] — parent global task.
