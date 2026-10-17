---
id: task-223-01-debug-break-shields-command
description: "cpp: rr.Debug.BreakShields console command next to rr.Debug.SpawnNear; drains the Shield attribute so OnShieldBroken fires, health kept."
parent: task-223-debug-break-shields
kind: task
status: in-progress
---
# rr.Debug.BreakShields

- goal: a console command that breaks the shield of living shield chasers without touching their health.
- agent: agent-cpp
- depends_on: none
- inputs: `Source/RogueRigs/Debug/RRVisualCheckCommands.cpp` (rr.Debug.SpawnNear and the other debug commands: follow their pattern), `Source/RogueRigs/Enemies/RRShieldChaser.h/.cpp`, the health attribute set (`URRHealthAttributeSet`, OnShieldBroken); `vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-shield-chaser.md`, `vault/reference/tech/systems/core/sys-cheats.md`, `vault/reference/agents/agent-cpp.md`.
- method: `rr.Debug.BreakShields [radius_uu]`: for every living ARRShieldChaser (within radius of the first living miner when given), take its Shield attribute to 0 through the ability system so the health set raises OnShieldBroken the normal way (same path as a hit: the Blueprint swaps materials and spawns NS_ShieldDebris); Health unchanged; log one line with the count. Debug builds only, like the other rr.Debug commands. No new test (not requested); update an existing test only if this change breaks it. Document the command where the other rr.Debug commands are documented.
- done_when: build succeeds with the editor closed; the existing tests that touch the shield chaser still pass if they are run as part of the usual build check; doc updated; commit by path.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-223-debug-break-shields]] — parent global task.
