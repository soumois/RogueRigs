---
id: task-207-02-capture
description: "cpp: In-game capture"
parent: task-207-intermission-hide-hud
kind: task
status: done
---
# capture

- goal: In-game capture (rendered -game, 1920 x 1080, L_ArenaM1) of the intermission screen, compared with the task-206-06 capture, sent to the owner.
- agent: cpp
- depends_on: task-207-01-hide-hud-code
- inputs: sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), intermission (vault/reference/gdd/run/intermission.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: PNG captures in Saved/Captures/ listed in the report.
- done_when: the capture shows the intermission screen with the fix; the orchestrator sends it to the owner.
- editor_state: closed
- token_ceiling: 40000
- model: opus
- tokens: input 30, cache write 47447, cache read 364395, output 1010
- runs: 1
- milestone: M2
- result: Rendered -game 1920 x 1080 captures sent to the owner: intermission shows only the widget (no minimap, bars, objectives, weapons, XP, no subtitle; a Kessler-Voss line active at extraction was cleared), level 2 shows the full HUD back (health 80/100 carried). Saved/Captures/task207_*.png.

## Relations
- [[task-207-intermission-hide-hud]] — parent global task.
