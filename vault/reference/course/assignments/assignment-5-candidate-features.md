---
id: assignment-5-candidate-features
description: "Preliminary scan of 2026-10-14: GDD features with no code yet, their GDD status, open fields, buildability. Not the agent's result; open to sanity-check its scan."
parent: assignment-5-goal-oriented-agent
kind: note
---
# Candidate features (preliminary, by keyword scan of `Source/RogueRigs/`)

The Code Architect must reach its own list. This one is the crew's sanity check. The feature to build is NOT chosen: it comes from the agent's score and the owner's sign-off.

| Candidate | GDD | Milestone | Code today | Open fields | Buildable without owner? |
|---|---|---|---|---|---|
| 4-direction fire (burst rifle) | [[upgrades]] approved: main shot on the aim, 3 weaker shots; [[sys-weapons]] says fire pattern in `FireOnce()` (provisional) | M2 | nothing found; piercing exists | strength of the 3 weaker shots: in no GDD node and in no data table (checked 2026-10-14) | no: one value from the owner (damage fraction); the agent reports it as open |
| Sound effects and music per alert tier | [[audio-direction]] provisional | M3 | no audio class or component at all | palette, mix rules, event list | no |
| Special-ore and boss objectives | [[objective-variants]] provisional; per-level objective `open` in [[run-structure]] | M2 | none | objective per level, boss entity | no; the boss is also in [[wishlist]] |
| Bark display | [[drone]] dialogue channel; barks exist as data, the subtitle subsystem exists ([[sys-subtitles]]) | M3 | barks not wired to subtitles | none found | yes |
| EMP, stuns machines | [[upgrades]] provisional | M3 if time allows | status effects exist ([[sys-status-effects]]) | stun duration | owner decides the value |

## Excluded
Anything only in [[wishlist]]: meta-progression shop, grenades, elements, per-weapon-type upgrades, player armor, void knockback.

## Expectation (not a decision)
4-direction fire depends only on built systems and serves pillar P2 (the weaker shots protect it), so it should rank high, but it needs one value from the owner ([[assignment-5-open-decisions]], item 12). Bark display is the only candidate with no open field. Audio would rank high on the instructor's volume-control logic but is blocked by open fields. Run structure (4 levels) and the damage numbers were not verified.

## Relations
- [[assignment-5-goal-oriented-agent]] — parent: brief and rubric.
- [[assignment-5-agent-design]] — how the agent gets its own list.
- [[gdd-index]] — where the specs are.
