---
id: task-026-01-wave-director
description: "Spawner director: calm, isolated robots, first pack at 22 s, rising multi-side flow, lull and horde front; one-type packs, tough robots first; tests."
parent: task-026-pressure
kind: task
status: done
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
- model: opus
- tokens: input 108, cache write 172799, cache read 5774082, output 99251
- runs: 1
- milestone: M2
- result: FRRWaveDirector scheduler + DirectorConfig on ARREnemySpawner (C++ defaults in MakeDefault): calm 10 s, lone chaser every 3 s, first pack 22 s, waves of 2–3 packs from distinct sides from 28 s, 6 → 3 s over 180 s, hordes (run-time trigger, provisional) fronts at 95 s and 175 s with 5 s lull and 4 s banner, 40–60 robots on one side over 2 s heavies first; cap 150. Events OnHordeIncoming/Lull/Started/Finished, OnDirectorPhaseChanged; horde queries. 5 Spawner.Director* tests; build 0, 163/163. To check: placed spawner MaxAlive override. Commits 1334d82, 28c9f76.

## Relations
- [[task-026-pressure]] — parent global task.
