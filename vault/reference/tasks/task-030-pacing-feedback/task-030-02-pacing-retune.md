---
id: task-030-02-pacing-retune
description: "Pacing bot over 3 seeds with the new director and speeds; XP curve retuned to the level targets; shots of packs with blended shooters."
parent: task-030-pacing-feedback
kind: task
status: done
---
# Pacing retune

- goal: level-up pacing still hits its targets after the director and speed changes.
- agent: cpp
- depends_on: task-030-01-director-speeds
- inputs: [[xp-level-up]], [[sys-progression]], task-028-03 result (bot rr.Pacing.Run), [[sys-cheats]], [[sys-automation-tests]]
- expected_output: FIRST (owner 2026-10-06, added while running): director shooter counter, at most 6 shooters alive (editable in DirectorConfig), a pack drawn at the cap gets chasers in place of its shooters, deterministic test; heavy pack weight 15 → 8 (chasers/shield chasers 92), heavy alive cap unchanged; shooter AttackRange 1200 → 960 uu in EnemyStats.csv (PreferredDistance 800 kept), shooter tests re-measured; docs sys-spawner-director. THEN: rr.Pacing.Run on 3 seeds, medians of L2, L5, L10 vs 0:30, 1:40, 3:10 ± 20 %; XPCurve.csv retuned if needed (formula documented, before/after table in sys-progression); tests updated; one -game shot run of the first pack and of a pack with shooters (open and describe: shooters blended, not in front, surround); proposed GDD curve text.
- done_when: medians within ±20 %; build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 110000
- model: opus
- tokens: input 172, cache write 501178, cache read 6688526, output 50840
- runs: 2
- milestone: M2
- result: Shooter cap 6 alive (ShooterMaxAlive, extras become chasers), heavy pack weight 8, shooter AttackRange 960 with the ring clamped to the range (fix: shooters stopped out of range at 996 uu). XP curve need(1) 12, then 12 × (L + 3): medians L2 26.4 s, L5 94.3 s, L10 175.1 s (pass). Shots: lone chasers from several sides, first pack, shooters mid or back of mixed packs, miner surrounded. New test DirectorShooterCap; build 0, 186/186; Progression.XPPickupAddsOne failed once (intermittent). Commits b8b0443, 6ba3021.

## Relations
- [[task-030-pacing-feedback]] — parent global task.
