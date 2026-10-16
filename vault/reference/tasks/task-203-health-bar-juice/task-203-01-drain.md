---
id: task-203-01-drain
description: "A C++ health status panel class drives the health bar: the shown fill glides to the real health instead of jumping."
parent: task-203-health-bar-juice
kind: task
status: todo
---
# Health bar class and smooth drain

- goal: A C++ health status panel class drives the health bar: the shown fill glides to the real health instead of jumping.
- agent: cpp
- depends_on: none
- inputs: wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), sys-hud-events (vault/reference/tech/systems/ui/sys-hud-events.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), naming (vault/reference/tech/conventions/naming.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), Source/RogueRigs/UI/RRHUDPanelWidget.h, Source/RogueRigs/Player/RRMiner.h
- spec: New class URRHealthStatusWidget : URRHUDPanelWidget in Source/RogueRigs/UI/, the future parent of WBP_HealthStatus. BindToMiner_Implementation binds the miner OnHealthChanged and OnDamaged (unbinds the previous miner; still calls the Blueprint hook so the vignette and game-over graphs keep working). BindWidgetOptional HealthBar (UProgressBar) and HealthText (UTextBlock). Text shows the real value at once ("round(Current) / round(Max)"). Shown fraction: Shown += (Target - Shown) * min(1, DrainRate * dt), DrainRate 10 per s, both ways (damage and heal); snap when the gap is under 0.001. Fill colour: FillColor property, default the current red (0.9, 0.05, 0.05). Values provisional, owner 2026-10-16, from the reference video. Creates the system doc node sys-hud-health-bar under sys-hud. Does not touch the Blueprint.
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
