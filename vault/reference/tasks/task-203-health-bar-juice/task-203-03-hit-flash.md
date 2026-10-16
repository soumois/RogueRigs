---
id: task-203-03-hit-flash
description: "On damage the health fill flashes towards white and fades back within about 0.1 s."
parent: task-203-health-bar-juice
kind: task
status: done
---
# Hit flash

- goal: On damage the health fill flashes towards white and fades back within about 0.1 s.
- agent: cpp
- depends_on: task-203-02-damage-chunk
- inputs: wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), sys-hud-events (vault/reference/tech/systems/ui/sys-hud-events.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), naming (vault/reference/tech/conventions/naming.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), Source/RogueRigs/UI/RRHUDPanelWidget.h, Source/RogueRigs/Player/RRMiner.h
- spec: On OnDamaged: Flash = 1; Flash -= FlashDecay * dt, FlashDecay 9 per s, floor 0. Fill colour = lerp(base fill colour, white, Flash). Distinct from the screen damage vignette of WBP_HealthStatus, which stays unchanged. Values provisional, owner 2026-10-16, from the reference video. Updates sys-hud-health-bar.
- expected_output: C++ in Source/RogueRigs/UI/, sys-hud-health-bar doc; one commit.
- done_when: build exit 0 with the editor closed; RogueRigs.HUD tests pass (only expectations this change breaks updated, no new test); every value of this node is an EditDefaultsOnly UPROPERTY default, none hard-coded; per-frame work runs only while an effect is running (no permanent tick); sys-hud-health-bar doc matches the code; committed by path.
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 24, cache write 35217, cache read 350277, output 1023
- runs: 1
- milestone: M1
- result: FlashColor white, FlashStrength 1, FlashDecay 9 per s; fill = lerp(GetBaseFillColor, FlashColor, FlashAlpha) via GetCurrentFillColor; vignette unchanged. Build 0, RogueRigs.HUD 13/13. Commit c88b43ba. Unverified on screen.

## Relations
- [[task-203-health-bar-juice]] — parent global task.
