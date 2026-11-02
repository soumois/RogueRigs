---
id: task-416-01-shake-mixing-falloff
description: "Cpp: additive mixing with cap and located shake with distance falloff in the shared camera shake"
parent: task-416-camera-shake-mixing-falloff
kind: task
status: done
---
# Camera shakes mix and fade with distance: cpp

- goal: Extend the shared camera shake: different preset classes add up, same class restarts at the stronger scale, summed translation capped at 12 uu; add a located call (epicenter, max distance, scale) with linear falloff 1 -> 0.5 at the max distance and nothing beyond, distance in 2D from the miner (camera view target).
- agent: cpp
- depends_on: none
- inputs: task-416-camera-shake-mixing-falloff (vault/reference/tasks/task-416-camera-shake-mixing-falloff.md), sys-camera-shake (vault/reference/tech/systems/core/sys-camera-shake.md), sys-player (vault/reference/tech/systems/core/sys-player.md), agent-cpp (vault/reference/agents/agent-cpp.md), task-310-camera-shake (vault/reference/tasks/task-310-camera-shake.md)
- expected_output: C++ changes in Source/RogueRigs/Camera/ (library, shake class or pattern, a camera-manager-side cap if needed), tunables EditDefaultsOnly or config with the decided values marked provisional owner 2026-11-02; commit task-416-01-shake-mixing-falloff; a change summary (public API, values, how the cap is enforced) for the doc subtask. Uncommitted edits of task-415 (RRAimIndicatorComponent, three test files, Config/DefaultEditor.ini) are not this task's: do not touch or commit them.
- done_when: Editor build green; the located call returns null and plays nothing beyond the max distance and scales 1 at the epicenter, 0.5 at the max distance (read in code); two different preset classes can play at once and their offsets add; a same-class call restarts at max(old, new) scale; the summed offset is capped at 12 uu; PlayCameraShake keeps its signature and chest behaviour; full RogueRigs automation suite with no new failure versus before; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 52, cache write 146613, cache read 1137040, output 20575
- usage: turns 26, context start 12k peak 74k, resumes 0, 17 min, units 0.40 M
- runs: 1
- milestone: M3
- started: 2026-11-02T18:05:08Z
- waiting_for: none
- result: Done, commit da0128f9: URRCameraShakeModifier (mixing, same-class max-scale restart, 12 uu cap), PlayCameraShakeAtLocation + GetDistanceFalloffScale (1 to 0.5, 0 beyond, 2D). Build green; suite 272 pass / 4 fail same before and after (task-415 test edits). Not verified in play.

## Relations
- [[task-416-camera-shake-mixing-falloff]] — parent global task.
