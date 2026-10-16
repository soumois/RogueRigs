---
id: task-203-04-shake
description: "On damage the health bar shakes a few pixels for 0.15 s."
parent: task-203-health-bar-juice
kind: task
status: in-progress
---
# Hit shake

- goal: On damage the health bar shakes a few pixels for 0.15 s.
- agent: cpp
- depends_on: task-203-03-hit-flash
- inputs: wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), sys-hud-events (vault/reference/tech/systems/ui/sys-hud-events.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), naming (vault/reference/tech/conventions/naming.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), Source/RogueRigs/UI/RRHUDPanelWidget.h, Source/RogueRigs/Player/RRMiner.h
- spec: BindWidgetOptional HealthBarRoot (UWidget holding the chunk and the bar). On OnDamaged: render translation is a random X and Y offset within ShakeAmplitude 4 px, re-rolled each frame, for ShakeDuration 0.15 s; amplitude decays linearly to 0 over the duration (decay shape provisional, accepted by the owner "ok" 2026-10-16). A new hit restarts it. Values provisional, owner 2026-10-16, from the reference video. Updates sys-hud-health-bar.
- expected_output: C++ in Source/RogueRigs/UI/, sys-hud-health-bar doc; one commit.
- done_when: build exit 0 with the editor closed; RogueRigs.HUD tests pass (only expectations this change breaks updated, no new test); every value of this node is an EditDefaultsOnly UPROPERTY default, none hard-coded; per-frame work runs only while an effect is running (no permanent tick); sys-hud-health-bar doc matches the code; committed by path.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-203-health-bar-juice]] — parent global task.
