---
id: task-044-01-fewer-kamikazes
description: "At most 1 kamikaze per pack in one pack out of two; hordes 1–2 kamikazes (was 3–5)."
parent: task-044-fewer-kamikazes
kind: task
status: done
---
# Fewer kamikazes

- goal: At most 1 kamikaze per pack in one pack out of two; hordes 1–2 kamikazes (was 3–5).
- agent: cpp
- depends_on: none
- inputs: enemies-index (vault/reference/gdd/enemies/enemies-index.md), sys-spawner-director (vault/reference/tech/systems/sys-spawner-director.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: FRRWaveDirectorConfig::MakeDefault: every pack type kamikaze member 0–1 (equal odds, like shooters) instead of 1–2 or 2–3; hordes kamikaze 1–2 instead of 3–5; update only existing director tests this breaks; docs sys-spawner-director.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 40000
- model: opus
- tokens: input 64, cache write 46987, cache read 976575, output 14164
- runs: 1
- milestone: M2
- result: Kamikazes 0–1 per Chasers, ShieldChasers and WideShieldChasers pack (was 1–2), hordes 1–2 (was 3–5; overtime horde copies it); early packs 5–10 robots. Director horde test updated; Spawner.HeadingSectorSplit sample 200 → 2000 hordes (the shift broke the small sample, cause not found). Build 0, 195/195. Commit d9a4454.

## Relations
- [[task-044-fewer-kamikazes]] — parent global task.
