---
id: task-103-01-hud-events-only
description: "C++ events for the minimap and locator and a one-shot vignette warm-up, so the HUD no longer polls every frame."
parent: task-103-hud-events-only
kind: task
status: in-progress
---
# HUD per-frame polling to events: C++ events

- goal: The HUD parts that still poll every frame (minimap redraw and texture, locator arrow, vignette warm-up) react to events instead; only genuinely continuous values keep a per-frame update.
- agent: cpp
- depends_on: task-094-10-hud-cpp-lookups
- inputs: hud-refactor-audit (vault/reference/tech/blueprints/hud-refactor-audit.md), hud-refactor-audit-core (vault/reference/tech/blueprints/hud-refactor-audit-core.md), hud-refactor-audit-run (vault/reference/tech/blueprints/hud-refactor-audit-run.md), hud-refactor-missing-events (vault/reference/tech/blueprints/hud-refactor-missing-events.md), sys-hud (vault/reference/tech/systems/sys-hud.md), sys-hud-events (vault/reference/tech/systems/sys-hud-events.md), sys-hud-minimap (vault/reference/tech/systems/sys-hud-minimap.md), sys-hud-pressure (vault/reference/tech/systems/sys-hud-pressure.md), wbp-minimap-locator (vault/reference/tech/blueprints/wbp-minimap-locator.md), perf-trace-real-game (vault/reference/tech/performance/perf-trace-real-game.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md)
- expected_output: owner rule: what can be an event is an event; only values that really change every frame (the reload and turret recharge progress, the vignette fade while it runs) stay per-frame. Read the audit (column "needs a per-frame update and why") and what is still polled after tasks 094-02..10: (1) URRHUDWidget::NativeTick still calls Minimap->Tick every frame and the WBP_MinimapLocator Blueprint runs a 0.016 s timer all the time and polls GetMinimapTexture until valid (OnMinimapTextureReady is a Blueprint event the panel cannot bind): give the C++ minimap real delegates, `OnMinimapTextureReady(UTexture*)` as a dynamic multicast and `OnMinimapViewChanged` fired only when the minimap content or the miner cell/position view really changes (fog revealed, miner moved by more than a threshold, a marker target moved), so an idle miner costs nothing; (2) the locator arrow: expose what changes it (target changed: already OnLocatorTargetChanged; miner moved or turned relative to the target: derive from the same movement-change signal, with a data threshold) so the Blueprint does not recompute while nothing moves; (3) KeepVignetteWarm: make it a one-shot warm-up at HUD start (and when the vignette widget appears) instead of a per-frame call, keeping the task-055 behaviour (first hit visible in a packaged build); (4) anything else URRHUDWidget::NativeTick still does each frame: remove it or document why it must stay; after this task NativeTick should be empty or hold only a genuinely continuous value. Keep behaviour identical to the player. Do not edit Blueprints (editor closed): the Blueprint side is task-103-02. Tests: each delegate fires once per real change and not when nothing changes (idle miner = no OnMinimapViewChanged), the texture event fires once, the warm-up runs once, NativeTick does no per-frame work (assert by counters). Docs sys-hud, sys-hud-events, sys-hud-minimap, sys-hud-pressure, sys-automation-tests, the audit hub note. Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; NativeTick holds no per-frame polling except a justified continuous value; docs updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 70000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-103-hud-events-only]] — parent global task.
