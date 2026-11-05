---
id: task-432-player-shot-impact-sparks
description: "Global task: Small white-gold sparks where a player shot hits its target, through a pool or a shared system (style guide A8). Awaiting owner answers."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Player shot impact sparks

- request: Owner 2026-11-05, answering 'Je passe le lot 1 au planner ?': "Oui, lot 1". Proposal he accepted: "**1. Retours manquants** : Étincelles d'impact, flash blanc de 0,05 à 0,1 s sur le robot touché, burst et anneau du baril réutilisés pour le kamikaze." Formalised by the main session: "Lot 1 d'amélioration des FX, trois retours visuels manquants (une tâche globale par point). Contraintes : color-code et readability-rules (pas de rouge sur les FX joueur ni dans les explosions, orange = explose), coût à horde (perf-lead-debris-gpu). Le rendu est validé en jeu par le propriétaire." Point 1: "Étincelles d'impact quand un tir du joueur touche une cible (règle A8 du style guide : aucun système d'impact de balle n'existe aujourd'hui). Passer par le pool Niagara existant (RRNiagaraPoolSubsystem) ou un système partagé, jamais un système par balle sans pool.". Owner answer 2026-11-05 (relayed by the main session, verbatim): "Ok, tout par défaut" = the plan and the defaults of Q1, Q2, Q3, Q4 accepted.
- goal: A player shot that hits its target shows small white-gold sparks at the impact, at a bounded cost in a horde.
- acceptance: A1 in game, a miner or turret shot hitting a robot (shield included) or a barrel shows small white-gold sparks at the impact point, no red, no orange; A2 a shot stopped by rock or ore shows none, robot shots show none, the drone laser keeps its current dig sparks; A3 sparks go through the existing Niagara pool or one shared system, never an unpooled system per bullet; when no slot is free the spark is skipped and nothing else changes; A4 CPU sprites only, at most 8 sprites and at most 0.25 s per impact (provisional, owner tunes in game), smaller than the robot death puff; A5 damage, piercing and shot behaviour unchanged, existing tests pass, only expectations this change breaks are updated; A6 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it.
- gdd_check: Read art-style-guide (A8), color-code, readability-rules, destruction-rules, perf-lead-debris-gpu, milestones. In scope: M3 juice (VFX). No conflict with the GDD. Not stated: which targets and which shots count (Q1, Q2), size (Q3), cost measure (Q4). Destruction-rules: shots never break blocks and rock stops them, and NS_DigImpact already means digging, which is why the default leaves rock without sparks. Perf lead: an active GPU system cost about 68 µs against 16 to 24 µs for CPU, hence A4. No code read by the Planner; the main session's read-only findings are in its brief, not relied on here beyond the request.
- confirmed_by_owner: yes (2026-11-05, "Ok, tout par défaut")
- milestone: M2
- next_step: task-432-03-doc
- started: 2026-11-05T15:45:57Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-05 (owner "Ok, tout par défaut"): every default below is accepted; values provisional, owner 2026-11-05.
- Q1 Sur quelles cibles les étincelles ? Default: robots (bouclier compris) et barils ; un tir arrêté par la roche ou le minerai n'en montre pas, car les tirs ne cassent jamais la roche et des étincelles y ressembleraient à celles du laser de forage. L'autre choix est d'en mettre aussi sur la roche.
- Q2 Quels tirs ? Default: tous les tirs blanc-or, mineur et tourelles ; le laser du drone garde ses étincelles de forage, les tirs des robots n'en ont pas.
- Q3 Quelle taille ? Default: au plus 8 particules et 0,25 s par impact, blanc-or, valeurs provisoires que tu ajustes après les avoir vues en jeu.
- Q4 Mesure de coût à horde sur les trois tâches du lot ? Default: aucune mesure, le coût est borné par construction (pool, particules CPU, compte limité), et tu demandes un stress test si tu vois une baisse en jeu.

## Subtasks
1. task-432-01-spawn-on-hit (vault/reference/tasks/task-432-player-shot-impact-sparks/task-432-01-spawn-on-hit.md) — cpp: A qualifying shot hit requests one impact spark at the hit point through the pool or a shared system, nothing when none is free.
2. task-432-02-spark-system (vault/reference/tasks/task-432-player-shot-impact-sparks/task-432-02-spark-system.md) — bp: The white-gold impact spark Niagara system exists and is the one the game spawns.
3. task-432-03-doc (vault/reference/tasks/task-432-player-shot-impact-sparks/task-432-03-doc.md) — doc: Document the spark path, its pool bound and the new Niagara system.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-432-01-spawn-on-hit]] — step 1, cpp.
- [[task-432-02-spark-system]] — step 2, bp.
- [[task-432-03-doc]] — step 3, doc.
