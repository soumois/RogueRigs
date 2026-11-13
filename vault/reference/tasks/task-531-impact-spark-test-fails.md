---
id: task-531-impact-spark-test-fails
description: "Global task: make test RogueRigs.Weapons.ImpactSparkAsset pass, fixing the defect at its source (test or impact effect pool code) without changing the design."
parent: tasks-done-index
kind: task
status: done
---
# Test RogueRigs.Weapons.ImpactSparkAsset en échec

- request: Relay 2026-11-13 (famille : comportement): "le test automatique `RogueRigs.Weapons.ImpactSparkAsset` échoue, en mode -nullrhi comme en -RenderOffScreen. Fichier : Source/RogueRigs/Tests/RRWeaponTests.cpp, lignes ~1620-1636. Asserts qui échouent : "Acquire prewarmed the pool", "one spark requested" (0 obtenu au lieu de 1), "the spark played" (0 au lieu de 1). L'échec arrive avant tout rendu et n'a pas été causé par task-530 (qui n'a touché que les checks de compilation matériau, M_MuzzleFlash et M_LevelUpWave). Objectif : que ce test passe, en trouvant si le défaut est dans le test ou dans le code du système d'étincelles d'impact (pool / Niagara), sans changer le design voulu.". Owner answer 2026-11-13 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2 accepted.
- goal: RogueRigs.Weapons.ImpactSparkAsset passes in both -nullrhi and -RenderOffScreen, with the cause (test or impact effect code) identified and fixed at its source, and the in-game impact effect unchanged from the design in burst-rifle (green flat cartoon impact burst at each hit, task-465).
- acceptance: A1 RogueRigs.Weapons.ImpactSparkAsset passes in a -nullrhi automation run and in a -RenderOffScreen run, including the asserts "Acquire prewarmed the pool", "one spark requested" and "the spark played"; A2 the executor report states where the defect was (test or impact effect code / pool / Niagara) and why, with the evidence; A3 the fix is at that source: a test is changed only if it asserts something the current design (burst-rifle vfx_sfx, task-465) or the current code contract no longer wants, never weakened to pass (no removed assert without a stated reason, no skip, no RHI guard that hides a code defect); A4 the other RogueRigs.Weapons automation tests pass as they did before the change (no new failure); A5 the game build and the editor build compile; A6 no change to the look, colour, size, timing or count of the impact effect in game, so no capture is needed; if the fix touches runtime code, the report says so and why it cannot change what the player sees.
- gdd_check: Read: gdd-index, weapons-index, burst-rifle (vfx_sfx: green flat cartoon impact burst at each hit, provisional, owner 2026-11-05, task-465; "was green sparks"). No conflict: the request only asks the test to pass without changing the design. Note for the executor: the test name says "spark" while the design moved to an impact burst in task-465; whether the test is stale or the code regressed is for the executor to establish, not diagnosed here (no code read, protocol step 2). No open value needed (the burst rifle sound and final name are open but untouched). Milestone M1, in scope. No capture: nothing the player sees changes (A6). Decision asked: Q1 (where the reference lies if test and code disagree).
- verification_level: standard
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "ok")
- milestone: M2
- next_step: none
- started: 2026-11-13T18:08:13Z
- waiting_for: none
- executor_tokens: input 68, cache write 87260, cache read 2052905, output 13330
- executor_usage: runs 1, turns 34, context start 16k peak 87k, resumes 0, 5 min, units 0.38 M, USD 1.11
- planning_tokens: input 26, cache write 38050, cache read 336221, output 6896
- planning_usage: aabf7a43 "Planifier fix ImpactSparkAsset" 1/1 (tasks 531), units 0.12 M, USD 0.40
- result: Done. Done. No doc node needed an update: only the test RogueRigs.Weapons.ImpactSparkAsset changed (716bdb8e), no runtime code, no contract change of ARRProjectile or URRNiagaraPoolSubsystem; sys-weapons does not cite the test or NS_ShotImpactSpark, and bp-shot-impact-burst already states the rifle plays NS_ShotImpactBurstGreen. Lint clean.

## Questions
Decided 2026-11-13 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-13.
- Q1 Si le test et le code ne sont pas d'accord, la référence est le design actuel (l'éclat d'impact vert plat de task-465, burst-rifle) : on corrige le code s'il ne le respecte plus, et on ne met à jour le test que s'il vérifie un ancien comportement (étincelles d'avant task-465) ou un contrat que le code n'a plus ; jamais d'assert retiré ni de test sauté pour le faire passer. Default: oui, c'est la règle.
- Q2 Niveau de vérification : standard (le test passe dans les deux modes, les autres tests Weapons ne régressent pas, build jeu et éditeur ok, pas de capture car rien ne change à l'écran). Default: standard.

## Subtasks
1. task-531-01-fix-impact-spark-test (vault/reference/tasks/task-531-impact-spark-test-fails/task-531-01-fix-impact-spark-test.md) — cpp: Find whether RogueRigs.Weapons.ImpactSparkAsset or the impact effect pool / Niagara code is wrong, fix it at its source, make the test pass in -nullrhi and -RenderOffScreen.
2. task-531-02-doc (vault/reference/tasks/task-531-impact-spark-test-fails/task-531-02-doc.md) — doc: Update the system doc if the fix changed the impact effect code or its contract; otherwise record that no doc change was needed.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-531-01-fix-impact-spark-test]] — step 1, cpp.
- [[task-531-02-doc]] — step 2, doc.
