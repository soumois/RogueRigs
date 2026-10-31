---
id: task-415-shotgun-base-damage-10
description: "Global task: raise the shotgun base damage from 6 to 10 per pellet in WeaponStats.csv, tests and docs."
parent: tasks-to-do-index
kind: task
status: todo
---
# Shotgun base damage 10

- request: Owner, 2026-10-31, verbatim (French, relayed by the main session): "J'aimerais passer les dégâts de base du fusil à pompe à 10 et diminuer la vitesse de tir des tourelles de 20%." This task covers the first point (shotgun base damage to 10); the second point (turret fire rate -20 %) is task-416-turret-fire-rate-minus-20. Owner answers 2026-10-31 (relayed by the main session, verbatim): "1. par plomb 2. c'est rajouté 20% entre deux tirs pardon, donc si on est à maintenant 0.38 entre deux tirs on passerait à 0.45 entre deux tirs". Q1 = 10 per pellet; the plan is accepted.
- goal: The shotgun base damage is 10 per pellet (was 6), read from the `Shotgun` row of `Content/RogueRigs/Data/Tables/WeaponStats.csv`; everything else of the shotgun (7 pellets, pierce 3, crit 5 % x1.5, fire rate, magazine, reload, range, knockback) unchanged; damage upgrades and crits apply on the new base as before. Reading per the Questions.
- acceptance: CSV row `Shotgun` column `Damage` = 10, no other cell changed; C++ builds; existing shotgun tests (`RRShotgunTests.cpp`, `PelletDamage` and the texts that say 6) updated to 10 and the `RogueRigs.Weapons` shotgun tests pass, no new test; rifle and turret values unchanged; no .uasset touched (BP_BurstRifle, ABP_Miner and the shop WBP stay the owner uncommitted edits); sys-weapons doc updated by the doc subtask; gdd shotgun stats line says 10 (edit applied on confirmation); lint clean; committed by path.
- gdd_check: Read 2026-10-31: [[weapons-index]], [[shotgun]] (approved; stats provisional, owner 2026-10-05: 6 damage per pellet, 7 pellets, pierce 3, crit 5 % x1.5, 0.5 s between shots). No conflict: a base value change of a provisional stat. Open values needed: none. Ambiguity: "dégâts de base" can mean per pellet (the only damage stat, 7 pellets = 70 per full volley) or the whole volley (Q1). Milestone M1 weapon, tuning in the current work (M2). Tech read: [[sys-weapons]] (base values from `WeaponStats.csv` row `Shotgun`, Damage 6). Code read: `Source/RogueRigs/Tests/RRShotgunTests.cpp` `constexpr float PelletDamage = 6.f` and texts "Damage 6 per pellet", "6 damage", "3 x 6" break with the change.
- confirmed_by_owner: yes (2026-10-31, "1. par plomb")
- milestone: M2
- next_step: task-415-01-shotgun-damage-csv
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-10-31 (owner "1. par plomb").
- Q1. "Dégâts de base à 10": 10 per pellet (each of the 7 pellets deals 10, so 70 per full volley, was 42), or 10 for the whole volley (about 1.43 per pellet)? Default: 10 per pellet. Answer: 10 per pellet.

## Subtasks
1. task-415-01-shotgun-damage-csv (vault/reference/tasks/task-415-shotgun-base-damage-10/task-415-01-shotgun-damage-csv.md) — cpp: set the `Shotgun` row Damage 6 -> 10 in WeaponStats.csv and update the shotgun test expectations
2. task-415-02-doc-shotgun-damage (vault/reference/tasks/task-415-shotgun-base-damage-10/task-415-02-doc-shotgun-damage.md) — doc: update sys-weapons (Shotgun row damage 10)

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-415-01-shotgun-damage-csv]] — step 1, cpp.
- [[task-415-02-doc-shotgun-damage]] — step 2, doc.
- [[shotgun]] — the gdd stats line this task changes.
- [[task-416-turret-fire-rate-minus-20]] — the second point of the same owner message.
