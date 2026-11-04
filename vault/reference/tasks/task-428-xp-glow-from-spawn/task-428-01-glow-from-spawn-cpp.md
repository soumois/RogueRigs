---
id: task-428-01-glow-from-spawn-cpp
description: "Cpp: ARRPickup bGlowDuringPop: glow target 1 from life start"
parent: task-428-xp-glow-from-spawn
kind: task
status: done
---
# XP pickup glow disc on from spawn: cpp

- goal: Add UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Glow", meta = (EditCondition = "bGlowFade")) bool bGlowDuringPop to ARRPickup (default per the global goal and Q4, header comment citing task-428 and owner 2026-11-04 provisional). In StartLife, after ResetGlowFade, when bGlowDuringPop set the glow target to 1 (fade-in over GlowFadeDuration from spawn; if Q2 says instant, write 1 at once). Keep the landing and ReturnToRest SetGlowFadeTarget(1), and Magnet / Collected / pool release as they are unless Q1 says otherwise. Update the bGlowFade and class header comments. No new test (not requested); fix only existing test expectations this change breaks.
- agent: cpp
- depends_on: none
- inputs: Source/RogueRigs/Pickups/RRPickup.h, Source/RogueRigs/Pickups/RRPickup.cpp, agent-cpp (vault/reference/agents/agent-cpp.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), task-428-xp-glow-from-spawn (vault/reference/tasks/task-428-xp-glow-from-spawn.md) (global request, goal and decided Questions)
- expected_output: RRPickup.h/.cpp changed and committed; change summary (property name, default, the lines changed, commit hash) for the doc subtask.
- done_when: The editor target builds without error; the existing pickup automation tests (RogueRigs.PulseDesync and any RogueRigs pickup tests) pass; with bGlowFade and bGlowDuringPop the glow target is 1 from StartLife; with bGlowDuringPop false the code path equals task-148.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 24, cache write 71008, cache read 623463, output 6462
- usage: turns 12, context start 12k peak 71k, resumes 0, 4 min, units 0.18 M
- runs: 1
- milestone: M2
- started: 2026-11-04T18:53:53Z
- waiting_for: none
- result: ARRPickup bGlowDuringPop (default true) added, StartLife sets glow target 1 after ResetGlowFade; build ok, RogueRigs.Pickups + PickupPool 14 tests pass; commit 2d3942d6

## Relations
- [[task-428-xp-glow-from-spawn]] — parent global task.
