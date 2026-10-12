---
id: task-104-01-hud-regressions
description: "Two Blueprint regressions of the HUD split are fixed: the damage vignette never fades and WBP_XPStatus divides by zero."
parent: task-104-hud-regressions
kind: task
status: done
---
# HUD split regressions: vignette fade and XP divide by zero

- goal: Two Blueprint regressions of the HUD split are fixed: the damage vignette never fades and WBP_XPStatus divides by zero.
- agent: bp
- depends_on: none
- inputs: wbp-health-status (vault/reference/tech/blueprints/wbp-health-status.md), wbp-xp-status (vault/reference/tech/blueprints/wbp-xp-status.md), wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), hud-refactor-audit (vault/reference/tech/blueprints/hud-refactor-audit.md), sys-hud-pressure (vault/reference/tech/systems/sys-hud-pressure.md), sys-hud-events (vault/reference/tech/systems/sys-hud-events.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md)
- expected_output: task-094-10 ran the full headless suite: 236 of 237 pass. The failing test is RogueRigs.Integration.PressureVignetteOnHit (Source/RogueRigs/Tests/RRPressureIntegrationTests.cpp, do not change its expectation): the red damage flash set by a hit never fades. The test read the WBP_HealthStatus variables after 60 frames: the FadeStep looping timer does run (LastFadeTime equals the world time) but HitHoldRemaining stays at 0.150 and HitFlash at 0.44, so FadeStep computes a time step of zero; in the real game the flash would stay on. Diagnose in WBP_HealthStatus (saved asset, via MCP read_graph_dsl): the delta is probably world time minus LastFadeTime where LastFadeTime is updated before the subtraction, or the timer handler does not use the engine delta; fix it so the hold (0.15 s at full strength, owner rule readability-rules) then decay works as the old UpdateVignette did (see wbp-health-status, sys-hud-pressure and the old behaviour in wbp-hud history), using GetWorldDeltaSeconds or the timer interval consistently; keep the self-stopping timer. Also WBP_XPStatus logs "Divide by zero" in headless runs: guard the division (the XP bar percent when XP-to-next-level is 0, as the old HUD did) so the log is clean and the bar shows 0. Then re-run is not possible for you (the Orchestrator reruns the test with the editor closed): verify by reading back the graph and, if you can, by computing one fade step by hand from the values. Compile 0 errors, save (dirty-asset trick, check git status shows the uasset changed), update wbp-health-status and wbp-xp-status notes, no screenshot calls. MCP via JSON-RPC to http://127.0.0.1:8000/mcp with small outputs, a few large write_graph_dsl batches, stray events deleted by connection count; commit by name after each fix (never git add -A; untracked .claude/agents/3d.md, art.md, Tools/art/ and Art/ belong to someone else). 40-turn limit.
- done_when: both fixes compiled 0 errors and saved; the fade logic reads back correct (hold then decay with a non-zero step); the XP division guarded; docs updated; lint clean; committed. The Orchestrator then reruns PressureVignetteOnHit headless.
- editor_state: open
- token_ceiling: 40000
- model: sonnet
- tokens: input 38, cache write 41268, cache read 531715, output 8757
- runs: 1
- milestone: M2
- result: Vignette FadeStep used pure nodes re-evaluated after SetLastFadeTime so dt was 0: now dt = GetWorldDeltaSeconds; XP percent division behind a Branch. Compiled and saved; PressureVignetteOnHit rerun in task-103-01. Commits 340e42c, e2750e0.

## Relations
- [[task-104-hud-regressions]] — parent global task.
