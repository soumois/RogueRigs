---
id: task-434-kamikaze-burst-and-ring
description: "Global task: The kamikaze explosion gets the Niagara burst and the ground ring of the barrel, scaled to its own radius. Awaiting owner answer."
parent: tasks-to-do-index
kind: task
status: todo
---
# Kamikaze explosion plays the barrel burst and ring

- request: Owner 2026-11-05, answering 'Je passe le lot 1 au planner ?': "Oui, lot 1". Proposal he accepted: "**1. Retours manquants** : Étincelles d'impact, flash blanc de 0,05 à 0,1 s sur le robot touché, burst et anneau du baril réutilisés pour le kamikaze." Formalised by the main session: "Lot 1 d'amélioration des FX, trois retours visuels manquants (une tâche globale par point). Contraintes : color-code et readability-rules (pas de rouge sur les FX joueur ni dans les explosions, orange = explose), coût à horde (perf-lead-debris-gpu). Le rendu est validé en jeu par le propriétaire." Decision asked: "Une décision te revient dans le lot 1 : donner au kamikaze le burst et l'anneau du baril, ce que la doc marque comme « not decided ». Je te le conseille, car le burst s'adapte déjà au rayon de l'explosion." Recorded as accepted: owner 2026-11-05, "Oui, lot 1". Point 3: "Explosion du kamikaze : lui donner le burst et l'anneau au sol du baril (aujourd'hui la ligne Kamikaze de ExplosionStats.csv pointe vers BP_ExplosionFX, sphère seule). NS_BarrelExplosion s'adapte déjà à User.Radius.". Owner answer 2026-11-05 (relayed by the main session, verbatim): "Ok, tout par défaut" = the plan and the defaults of Q1 accepted.
- goal: The kamikaze explosion shows the barrel's burst and ground ring at the kamikaze's own size, with every game rule unchanged.
- acceptance: A1 in game, every kamikaze explosion (proximity, killed by the player after its fuse, killed by a chain) shows the orange sphere, the Niagara burst and the ground ring, sized to its visual radius of 327.6 uu; A2 the barrel explosion looks as before; A3 radius, damage, miner damage, terrain effect, chain delay and camera shake of both rows unchanged; A4 no red in the explosion, orange and grey smoke only, the ring stays orange; A5 the kamikaze ring does not bring back the defect closed by task-431, checked from that task's cause without a packaged build; A6 existing tests pass, only expectations this change breaks are updated; A7 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it.
- gdd_check: Read explosions, enemy-kamikaze, sys-explosions, bp-explosion-fx-barrel, bp-barrel-explosion, color-code, readability-rules, milestones. In scope: M3 juice; A10 and orange = explodes agree. Decision recorded: reuse accepted, owner 2026-11-05, "Oui, lot 1" (bp-explosion-fx-barrel said not decided). Second child or row change is the Executor's choice. Per sys-explosions the CSV is read directly at runtime, no DataTable asset to reimport. Cost: the burst is not pooled (95 CPU sprites, 1.5 s), but explosions resolve one per 0.08 s at most and a horde holds 1 or 2 kamikazes, so it stays bounded; Q4 of task-432 covers any measure.
- confirmed_by_owner: yes (2026-11-05, "Ok, tout par défaut")
- milestone: M2
- next_step: task-434-01-row-fx
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-11-05 (owner "Ok, tout par défaut"): every default below is accepted; values provisional, owner 2026-11-05.
- Q1 Le kamikaze a-t-il exactement le rendu du baril, à sa taille ? Default: oui, mêmes émetteurs et mêmes comptes de particules, tailles réduites par son rayon (327,6 uu contre 735), sans réglage propre avant que tu l'aies vu en jeu.

## Subtasks
1. task-434-01-row-fx (vault/reference/tasks/task-434-kamikaze-burst-and-ring/task-434-01-row-fx.md) — bp: The Kamikaze row plays the burst and the ring.
2. task-434-02-doc (vault/reference/tasks/task-434-kamikaze-burst-and-ring/task-434-02-doc.md) — doc: Record the decision and what the Kamikaze row now plays.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-434-01-row-fx]] — step 1, bp.
- [[task-434-02-doc]] — step 2, doc.
