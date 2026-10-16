---
id: task-203-05-punch
description: "On damage the health bar grows in height then springs back to its normal size."
parent: task-203-health-bar-juice
kind: task
status: done
---
# Hit punch

- goal: On damage the health bar grows in height then springs back to its normal size.
- agent: cpp
- depends_on: task-203-04-shake
- inputs: wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), sys-hud-events (vault/reference/tech/systems/ui/sys-hud-events.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), naming (vault/reference/tech/conventions/naming.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), Source/RogueRigs/UI/RRHUDPanelWidget.h, Source/RogueRigs/Player/RRMiner.h
- spec: On OnDamaged: HealthBarRoot render scale Y = PunchScale 1.3, then a damped spring brings it back to 1 (PunchStiffness 400, PunchDamping 25: a small overshoot, settled in about 0.25 s; spring values provisional, accepted by the owner "ok" 2026-10-16). Pivot at the centre (0.5, 0.5). Scale X stays 1. Combines with the shake translation in one render transform. PunchScale provisional, owner 2026-10-16, from the reference video. Updates sys-hud-health-bar.
- expected_output: C++ in Source/RogueRigs/UI/, sys-hud-health-bar doc; one commit.
- done_when: build exit 0 with the editor closed; RogueRigs.HUD tests pass (only expectations this change breaks updated, no new test); every value of this node is an EditDefaultsOnly UPROPERTY default, none hard-coded; per-frame work runs only while an effect is running (no permanent tick); sys-hud-health-bar doc matches the code; committed by path.
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 28, cache write 46962, cache read 520211, output 7154
- runs: 1
- milestone: M1
- result: PunchScale 1.3, PunchStiffness 400, PunchDamping 25, substeps of 1/120 s (max 60), snap thresholds 0.001 and 0.01/s, pivot (0.5, 0.5) set from C++ in NativeOnInitialized; SnapBarEffects shared reset. Node simulation: min 0.981, within 0.008 at 0.25 s, exact snap at about 0.48 s. Build 0, RogueRigs.HUD 13/13. Commit ee56a080. Unverified on screen.

## Relations
- [[task-203-health-bar-juice]] — parent global task.
