---
id: task-203-02-damage-chunk
description: "On damage, an off-white chunk keeps the lost health visible for a moment, then shrinks to the shown fill."
parent: task-203-health-bar-juice
kind: task
status: done
---
# Damage chunk

- goal: On damage, an off-white chunk keeps the lost health visible for a moment, then shrinks to the shown fill.
- agent: cpp
- depends_on: task-203-01-drain
- inputs: wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), sys-hud-events (vault/reference/tech/systems/ui/sys-hud-events.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), naming (vault/reference/tech/conventions/naming.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), Source/RogueRigs/UI/RRHUDPanelWidget.h, Source/RogueRigs/Player/RRMiner.h
- spec: BindWidgetOptional DamageChunkBar (UProgressBar, drawn behind HealthBar). On OnDamaged: the chunk stays at its value for ChunkDelay 0.4 s (each new hit restarts the delay), then Chunk += (Shown - Chunk) * min(1, ChunkRate * dt), ChunkRate 7 per s. The chunk is never below Shown (a heal sets Chunk = Shown when Shown passes it). Chunk fill colour ChunkColor #fff0e0 (off-white; the video tried red #f25a5a and pink #ff99cc and kept this one). Values provisional, owner 2026-10-16, from the reference video. Updates sys-hud-health-bar.
- expected_output: C++ in Source/RogueRigs/UI/, sys-hud-health-bar doc; one commit.
- done_when: build exit 0 with the editor closed; RogueRigs.HUD tests pass (only expectations this change breaks updated, no new test); every value of this node is an EditDefaultsOnly UPROPERTY default, none hard-coded; per-frame work runs only while an effect is running (no permanent tick); sys-hud-health-bar doc matches the code; committed by path.
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 20, cache write 31207, cache read 252692, output 477
- runs: 1
- milestone: M1
- result: DamageChunkBar (BindWidgetOptional), ChunkDelay 0.4 s restarted per hit, ChunkRate 7, ChunkColor #fff0e0 (sRGB), chunk never below shown, getter GetChunkFraction; settle waits for the hold. Build 0, RogueRigs.HUD 13/13. Commit 2b3e800d. Unverified on screen; HealthBar background must be transparent (task-203-07).

## Relations
- [[task-203-health-bar-juice]] — parent global task.
