---
id: task-026-01-wave-director
description: "Spawner director: calm, isolated robots, first pack at 22 s, rising multi-side flow, lull and horde front; one-type packs, tough robots first; tests."
parent: task-026-pressure
kind: task
status: in-progress
---
# Wave director

- goal: enemies arrive with a DRGS-like tension curve.
- agent: cpp
- depends_on: task-026-00-landing-clearance
- inputs: [[enemies-index]], [[level-flow]], [[sys-spawner]], [[sys-enemies]], [[sys-objectives]], [[cpp-standard]], [[sys-automation-tests]], [[drgs-analysis]]
- expected_output: director on ARREnemySpawner (data-driven, editable): 0–10 s nothing; 10–22 s one robot every 3 s; first pack at 22 s (8–10 chasers from one side); flow: packs of one type (chasers 8–15, kamikazes 2–3 with them, shooters 3–5, heavies 2–3 placed at the front of a mixed pack) from 2–3 sides, interval shrinking from 6 s to 3 s over 3 min; hordes at 1:35 and 2:55 of level time (or alert tiers 40 % and 75 %): lull 5 s (no spawn), banner event, horde 4 s later = 40–60 robots spawned as a front along one side over 2 s, heavies first; cap 150; spawn just outside the view; weights per phase in data; docs sys-spawner, enemies-index note for the GDD. Tests `RogueRigs.Spawner.Director*` (phase times, horde size and side, pack homogeneity, cap).
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 130000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-026-pressure]] — parent global task.
