---
id: task-012-xp-level
description: "Global task: slower bigger tougher kamikaze, slower chasers, render-only block shake, then XP and level-up without upgrade cards, HUD bar and tests."
parent: tasks-index
kind: task
status: review
---
# XP and level-up (no upgrades yet)

- request: "Applique ces modifications et lance la tâche pour l'XP et le level up, mais sans les améliorations pour l'instant: les kamikazes doivent être lents et gros; lorsqu'un bloc tremble, ça doit juste être son rendu, pas sa collision; ralentis de 20 % les chasseurs." Then: kamikaze 3 to 4 × a chaser's health; plan confirmed (2026-10-02).
- goal: kamikazes are slow (300 uu/s), big (1.1 × the miner) and tough (100 HP); chasers run at 360 uu/s; shaking blocks never push the miner; killed robots' XP cubes fill a cyan XP bar, levels rise on the 5 × level curve and "LEVEL UP" shows for 1 s.
- acceptance: (1) build exits 0; all `RogueRigs.*` headless tests pass, new ones included; (2) a shaking block keeps its collision still (test); (3) XP collected × `XPGain` adds to the progression, levels follow the CSV curve with carry-over; (4) HUD shows XP, level and level-up; no divide-by-zero warning at PIE start; (5) docs `sys-progression` (new), `sys-grid`, `sys-enemies`, `sys-pickups`, `sys-hud`, Blueprint docs; lint clean; every subtask committed; (6) owner plays it.
- gdd_check: read [[xp-level-up]], [[upgrades]], [[player-stats]], [[enemy-kamikaze]], [[enemy-chaser]], [[drone]], [[sys-gas-core]] (XP and level are not attributes: no upgrade changes them). Open values set by the owner (2026-10-02): kamikaze 100 HP, 300 uu/s, 1.1 × size; chaser 360 uu/s; XP curve 5 × level; level-up feedback and HUD layout. Out: upgrade cards, pause on choice, heal and ore pickup effects. TaskCreate and TaskUpdate are not available in this session: progress is tracked in these nodes.
- confirmed_by_owner: yes (2026-10-02, all subtasks pre-approved)
- milestone: M1
- next_step: none (owner: play it)
- result: acceptance 1 to 5 passed: 58/58 headless tests (render-only shake keeps collision, progression with carry-over and XPGain, XP cubes, 10-hit kamikaze, 5 kills → level 2); PIE 12 s with no warning. Owner to play it (6): HUD layout, kamikaze size, chaser speed. About 390k tokens over four subtasks.

## Subtasks
1. task-012-01-tuning-shake — cpp: kamikaze and chaser values, render-only shake, tests.
2. task-012-02-progression — cpp: XP and level component, CSV curve, XP pickups, HUD events, tests.
3. task-012-03-bp-hud-kamikaze — bp: bigger kamikaze visuals, XP bar, level, LEVEL UP text, HUD fix, PIE.
4. task-012-04-tests-integration — cpp: kill, collect, level up with the final Blueprints.

## Relations
- [[tasks-index]] — parent index.
- [[task-012-01-tuning-shake]] — step 1, cpp.
- [[task-012-02-progression]] — step 2, cpp.
- [[task-012-03-bp-hud-kamikaze]] — step 3, bp.
- [[task-012-04-tests-integration]] — step 4, cpp.
