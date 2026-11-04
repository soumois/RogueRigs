---
id: task-428-01-glow-from-spawn-cpp
description: "Cpp: ARRPickup bGlowDuringPop: glow target 1 from life start"
parent: task-428-xp-glow-from-spawn
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-428-xp-glow-from-spawn]] — parent global task.
