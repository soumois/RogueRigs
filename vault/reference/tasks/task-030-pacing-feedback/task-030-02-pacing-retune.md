---
id: task-030-02-pacing-retune
description: "Pacing bot over 3 seeds with the new director and speeds; XP curve retuned to the level targets; shots of a pack arriving with blended shooters."
parent: task-030-pacing-feedback
kind: task
status: todo
---
# Pacing retune

- goal: level-up pacing still hits its targets after the director and speed changes.
- agent: cpp
- depends_on: task-030-01-director-speeds
- inputs: [[xp-level-up]], [[sys-progression]], task-028-03 result (bot rr.Pacing.Run), [[sys-cheats]], [[sys-automation-tests]]
- expected_output: rr.Pacing.Run on 3 seeds, medians of L2, L5, L10 vs 0:30, 1:40, 3:10 ± 20 %; XPCurve.csv retuned if needed (formula documented, before/after table in sys-progression); tests updated; one -game shot run of the first pack and of a pack with shooters (open and describe: shooters blended, not in front, surround); proposed GDD curve text.
- done_when: medians within ±20 %; build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 110000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-030-pacing-feedback]] — parent global task.
