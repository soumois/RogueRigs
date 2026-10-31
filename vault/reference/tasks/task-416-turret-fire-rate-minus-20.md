---
id: task-416-turret-fire-rate-minus-20
description: "Global task: turret shots every 0.4 s (FireRate 3.3 to 2.5 shots/s) in WeaponStats.csv, turret tests and docs; owner value."
parent: tasks-to-do-index
kind: task
status: todo
---
# Turret fire rate: 0.4 s between shots

- request: Owner, 2026-10-31, verbatim (French, relayed by the main session): "J'aimerais passer les dégâts de base du fusil à pompe à 10 et diminuer la vitesse de tir des tourelles de 20%." This task covers the second point (turret fire rate -20 %); the first point (shotgun base damage to 10) is task-415-shotgun-base-damage-10. Owner answers 2026-10-31 (relayed by the main session, verbatim): "1. par plomb 2. c'est rajouté 20% entre deux tirs pardon, donc si on est à maintenant 0.38 entre deux tirs on passerait à 0.45 entre deux tirs". Q2 = 20 % more time between shots (not shots/s x 0.8); his example starts from 0.38 s, but 0.38 s was the result of the other option: today the interval is 0.30 s (3.3 shots/s), so +20 % gives 0.36 s, while his figure 0.45 s is about +50 % (Q3 asks which one). Owner answer to Q3 2026-10-31 (relayed by the main session, verbatim): "Ok, passe le temps entre deux tirs à 0.4 ignore le pourcentage." = 0.4 s between turret shots, `FireRate` 2.5 shots/s (1 / 0.4), the percentage ignored.
- goal: Turrets fire slower: the turret launcher base `FireRate` (copied by every turret at placement) is 2.5 shots/s (one shot every 0.4 s instead of 0.303 s; owner value, Q3), read from the `TurretLauncher` row of `Content/RogueRigs/Data/Tables/WeaponStats.csv`; damage, pierce, crit, charges, recharge, lifetime and range unchanged; fire-rate upgrades apply on the new base as before.
- acceptance: CSV row `TurretLauncher` column `FireRate` = 2.5, no other cell changed; C++ builds; existing turret tests (`RRTurretTests.cpp`: `TurretFireRate`, texts "3.3", the 1 s cadence test expecting shots at 0, 0.30, 0.61, 0.91 s) updated to 2.5 shots/s (cadence: shots at 0, 0.4, 0.8 s in 1 s) and the turret tests pass, no new test; shotgun and rifle values unchanged; no .uasset touched; sys-turret doc updated by the doc subtask; gdd turret stats line says 2.5 shots/s, 0.4 s between shots (applied at confirmation); lint clean; committed by path.
- gdd_check: Read 2026-10-31: [[weapons-index]], [[turret]] (approved; stats provisional, owner 2026-10-05: 3.3 shots/s; a placed turret copies every launcher stat at placement). No conflict: a base value change of a provisional stat. Open values needed: none. Ambiguity: "diminuer la vitesse de tir de 20 %" read as shots per second x 0.8 = 2.64, or as the time between shots +20 % = 2.75 shots/s (Q2: owner chose +20 % between shots; his example base 0.38 s is wrong, today 0.30 s, so Q3 asked 0.36 s or his 0.45 s; Q3 answer: 0.4 s, 2.5 shots/s). Milestone M2, current work. Tech read: [[sys-turret]] (`WeaponStats.csv` row `TurretLauncher` FireRate 3.3), [[sys-weapons]] (turret attribute copy). Code read: `Source/RogueRigs/Tests/RRTurretTests.cpp` `constexpr float TurretFireRate = 3.3f`, texts "fire rate 3.3 shots/s", "copied fire rate 3.3" and the cadence comment at line ~428 break with the change.
- confirmed_by_owner: yes (2026-10-31, "Ok, passe le temps entre deux tirs à 0.4 ignore le pourcentage.")
- milestone: M2
- next_step: task-416-01-turret-fire-rate-csv
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-10-31 (owner "Ok, passe le temps entre deux tirs à 0.4 ignore le pourcentage.").
- Q2. "Diminuer la vitesse de tir des tourelles de 20 %": shots per second x 0.8 (3.3 -> 2.64 shots/s, 0.30 s -> 0.38 s between shots), or 20 % more time between shots (0.30 s -> 0.36 s, 2.75 shots/s)? Default: 2.64 shots/s. Answer 2026-10-31: 20 % more time between shots ("c'est rajouté 20% entre deux tirs").
- Q3. Today a turret fires every 0.30 s (3.3 shots/s); 0.38 s was the result of the option you did not pick. +20 % between shots gives 0.36 s (2.75 shots/s, 17 % fewer shots); your figure 0.45 s (2.22 shots/s, 33 % fewer shots) is +50 %. Which one? Default: 0.36 s (2.75 shots/s). Answer 2026-10-31: 0.4 s between shots, percentage ignored (`FireRate` 2.5).

## Subtasks
1. task-416-01-turret-fire-rate-csv (vault/reference/tasks/task-416-turret-fire-rate-minus-20/task-416-01-turret-fire-rate-csv.md) — cpp: set the `TurretLauncher` row FireRate 3.3 -> 2.5 (0.4 s between shots) in WeaponStats.csv and update the turret test expectations
2. task-416-02-doc-turret-fire-rate (vault/reference/tasks/task-416-turret-fire-rate-minus-20/task-416-02-doc-turret-fire-rate.md) — doc: update sys-turret (TurretLauncher row FireRate 2.5)

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-416-01-turret-fire-rate-csv]] — step 1, cpp.
- [[task-416-02-doc-turret-fire-rate]] — step 2, doc.
- [[turret]] — the gdd stats line this task changes.
- [[task-415-shotgun-base-damage-10]] — the first point of the same owner message.
