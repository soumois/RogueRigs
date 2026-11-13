---
id: task-526-chest-light-leftover
description: "Global task: Defect: the bonus chest light effect stays in the next level where no chest is."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Lumière du coffre restée du niveau précédent

- request: Owner 2026-11-13, play-test batch in PIE (12 points, family gros morceau split one point per global), with two screenshots (image 1 gameplay: HUD, alert bar C / B / ! / S, a chest light glow with no chest; image 2 Options, Sound tab, the "Sound" tab button drawn smaller than the other tabs). This point, verbatim: « Image 1 : J'ai l'effet lumineux pour du coffre qui est là, alors que le coffre n'est pas là. je pense que cela vient du coffre du niveau précédant. ( la particule n'a pas été nettoyée ) ». Owner answer 2026-11-13 (relayed by the main session, verbatim): "tout par défaut" = the plan accepted.
- goal: No chest light or particle effect remains in a level where no chest stands; the open chest still glows per objective-bonus-chest within its own level.
- acceptance: A1 open a chest in level N, extract, play level N+1: no chest light or effect at its former spot or elsewhere until a chest drops; A2 the chest opening effects of objective-bonus-chest unchanged within its level; A3 build green, existing tests pass; capture and doc done.
- gdd_check: Read objective-bonus-chest. No conflict: defect. The owner's cause (previous level's particle) is his hypothesis, not checked here. No code read.
- verification_level: standard
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "tout par défaut")
- milestone: M2
- next_step: task-526-01-cleanup-fix
- started: 2026-11-13T14:10:41Z
- waiting_for: none
- result: open

## Subtasks
1. task-526-01-cleanup-fix (vault/reference/tasks/task-526-chest-light-leftover/task-526-01-cleanup-fix.md) — cpp: chest effects removed with the level they belong to
2. task-526-02-capture (vault/reference/tasks/task-526-chest-light-leftover/task-526-02-capture.md) — capture: next level after an opened chest
3. task-526-03-doc (vault/reference/tasks/task-526-chest-light-leftover/task-526-03-doc.md) — doc: sys-objectives-console-chest: effect lifetime, if the fix changes how it is built

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-526-01-cleanup-fix]] — step 1, cpp.
- [[task-526-02-capture]] — step 2, capture.
- [[task-526-03-doc]] — step 3, doc.
