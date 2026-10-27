---
id: task-328-aim-chevrons
description: "Global task: show the miner's aim direction as a row of white-gold chevrons projected on the ground, direction only, stopping at rock."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Aim indicator: ground chevrons

- request: "Pour l'instant, je vais valider cette décision, prépare le plan d'intégration" (owner, 2026-10-27, via /task). Decision validated in the previous conversation (relayed by the main session, 2026-10-27): "piste 1", chevrons on the ground in front of the miner, white-gold, pointing in the aim direction; not a laser (red is enemy fire, a laser is too classic); about 5 chevrons, opacity decreasing with distance, projected on the floor like the ground telegraph; weapon-agnostic, owner explicit: "the miner will have several weapons, each with its own rate, range and spread", so direction only, no spread, no pulse per shot, no length tied to a weapon range; fixed length; never a dotted circle; turret preview later, out of scope. Owner answers (2026-10-27, relayed verbatim by the main session): "Q4: Laisser s'éteindre" (no brightness compensation for the light mask: the far chevrons fade out naturally) and "Q5: Pas de capture, lance juste l'éditeur que je puisse tester" (no `-game` capture, no screenshot comparison; after the build the main session launches the Unreal Editor and the owner tests himself). Q1, Q2, Q3, Q6 not answered: the proposed defaults apply, as the owner was told "ok" accepts them (main session reading, owner informed): Q1 stop at the first rock block kept; Q2 values of [[aim-indicator]] provisional, owner 2026-10-27; Q3 visible whenever the miner is alive, mouse and stick alike, hidden at death; Q6 images in `Art/Source/aim-indicator/`. His request to launch the editor to test is read as confirmation of the plan.
- goal: in game, a row of white-gold chevrons lies on the ground in front of the miner and always points where the miner aims (mouse or right stick), as specified in [[aim-indicator]].
- acceptance: build succeeds and existing tests pass; the component and material implement [[aim-indicator]] (follows the aim with mouse and stick, white-gold, opacity decreasing with distance, no change when the weapon fires or changes, stops at the first rock block on the aim line, hidden at death, no brightness compensation, no gameplay change); reference images in the repo; docs updated; committed; the Unreal Editor is launched for the owner to test (no capture, owner Q5). The look is judged by the owner, not verified by the crew.
- gdd_check: read [[gdd-pillars]] (passes P1 feedback and P2 manual aim), [[miner]] (mouse or stick aim; cannot shoot through rock; turret on the aimed spot), [[drone]] (aim never places the drone; drone laser colour open red vs white-gold: not blocking, the chevrons are a static ground shape, not a beam, and white-gold is the player shot colour either way), [[color-code]] and [[readability-rules]] (white-gold = player shots; red, orange, acid green, steel blue, rarity colours and dotted circles excluded: no conflict), [[ground-telegraph]] and [[sys-ground-telegraph]] (same decal technique; a decal climbs onto rock tops, another reason for the stop-at-rock rule), [[destruction-rules]] (projectiles stop on rock), [[sys-player-input-aim]] (`GetAimDirection()` exists; no aim indicator or cursor visual is documented), [[mpp-player-light-mask]] (screen darkener: at 400 uu the gain is about 0.28, the far chevrons would be dim), [[milestones]] (juice = M3, current milestone: in scope). Conflicts: none. Open values were asked as Q1 to Q6 and are decided (see request): stop at rock yes; mockup values provisional; visible while alive; no darkener compensation; no capture, editor launched; images in `Art/Source/aim-indicator/`. GDD node created and updated: [[aim-indicator]].
- confirmed_by_owner: yes (2026-10-27, "lance juste l'éditeur que je puisse tester", relayed by the main session)
- milestone: M3
- next_step: task-328-03-aim-indicator-component
- started: 2026-10-27T15:31:14Z
- waiting_for: editor closed for subtask 328-03 (cpp build)
- result: open

## Subtasks
1. task-328-01-reference-images (vault/reference/tasks/task-328-aim-chevrons/task-328-01-reference-images.md) — art: copy the owner-seen mockup and base screenshot into the repo and list them in aim-indicator.
2. task-328-02-chevron-material (vault/reference/tasks/task-328-aim-chevrons/task-328-02-chevron-material.md) — bp: build the deferred decal material M_AimChevrons with the parameter contract.
3. task-328-03-aim-indicator-component (vault/reference/tasks/task-328-aim-chevrons/task-328-03-aim-indicator-component.md) — cpp: aim indicator decal component on the miner (follows the aim, stops at rock, hidden at death); no capture.
4. task-328-04-docs (vault/reference/tasks/task-328-aim-chevrons/task-328-04-docs.md) — doc: create sys-aim-indicator, update sys-player and systems-combat-index.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-328-01-reference-images]] — step 1, art.
- [[task-328-02-chevron-material]] — step 2, bp.
- [[task-328-03-aim-indicator-component]] — step 3, cpp.
- [[task-328-04-docs]] — step 4, doc.
- [[aim-indicator]] — the GDD spec it implements.
