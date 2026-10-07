---
id: task-034-04-director-mix
description: "Wave director: 1–2 kamikazes per pack, rare shooters (none before 90 s, cap 4), packs of 6–10 robots until 90 s."
parent: task-034-fourth-test
kind: task
status: done
---
# Softer early waves

- goal: fewer kamikazes and shooters, smaller early packs.
- agent: cpp
- depends_on: task-034-03-docs
- inputs: [[enemies-index]], [[sys-spawner-director]], [[sys-automation-tests]], [[cpp-standard]]
- expected_output: FRRWaveDirectorConfig::MakeDefault: kamikazes 1–2 per pack; shooters at most 1, in one flow pack out of two, none before 90 s; ShooterMaxAlive 6 → 4; packs 6–10 robots until 90 s; update only the existing director tests these values break; docs sys-spawner-director; also fix sys-enemies MoveSpeed values to 180 for every robot (stale since task-033).
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: opus
- tokens: input 58, cache write 95042, cache read 2041524, output 26964
- runs: 2
- milestone: M2
- result: Flow packs before 90 s: chasers 5–8 with 1–2 kamikazes, heavies 2–3 with 4–7 chasers, shield packs 6–9 robots, no shooter; from 90 s new WideShieldChasers and WideHeavies with 0–1 shooter (equal odds: 30–35 of about 77 packs, 3.2–3.6 % of robots); ShooterMaxAlive 4; director tests updated; sys-enemies speeds 180. Build 0, 195/195. Commit 6c90be7.

## Relations
- [[task-034-fourth-test]] — parent global task.
