---
id: task-220-shield-chaser-integration
description: "Put the validated riot shield on the shield chaser in game, with a shield-held run, a shield bash and the shield vanishing when it breaks."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Shield chaser: shield integration and animation

- request: "Lance la tâche d'intégration et d'animation (je ne serai plus là, je validerai la texture demain, mais ça se change vite). L'éditeur est disponible. Lance ce que tu penses comme valeur par défaut concernant l'intégration du bouclier + animation, nous corrigerons demain si nécessaire." (owner 2026-10-17; earlier: same chassis as the chaser, a shield bash, the shield held in front while running)
- goal: BP_ShieldChaser shows the chaser body with the riot shield on its left forearm, runs holding it in front, attacks with a shield bash, and loses the shield (plain chaser clips) when the 30-point shield breaks.
- acceptance: a `-game` capture of a shield chaser running, bashing and after the break, sent to the owner; BP doc node rewritten.
- gdd_check: [[enemy-chaser]] (shield variant: 30-point pool from any direction, unchanged: a look change only), [[art-prompts-shield]], [[chaser-meshy-pipeline]]. Orchestrator defaults (owner away, to correct tomorrow): no C++ change (a Blueprint material swap keeps the chaser code untouched); the shield is bound to part 4 (ForearmHand_L) of one combined animated mesh; one 4-clip texture (shield run, shield bash, plain run, plain attack, same frame counts as the chaser so the 0.1 s wind-up and 0.5 s attack stay true); on break the Blueprint swaps materials (body to the plain clips, shield and emblem slots to an invisible instance), restored on pool reuse by RestoreVisualParts; the translucent shell is removed; no shield debris yet.
- confirmed_by_owner: yes (defaults delegated, 2026-10-17)
- milestone: M1
- next_step: task-220-03-bp-shield-chaser-wiring
- result: open

## Subtasks
- [[task-220-01-shield-clips-bake]] — 3D agent: shield poses, 4-clip bake, combined mesh with the shield on part 4, previews.
- [[task-220-02-shield-chaser-material]] — bp: import, material with the 4-clip layout and its instances.
- [[task-220-05-shield-pavise-pose]] — 3D agent: shield 20 % larger carried low like a pavise, head above it (owner feedback); runs after 220-02, before 220-03.
- [[task-220-06-shield-chaser-reimport]] — cpp: headless reimport of the rebaked mesh and anim texture, build settings, channel 2 check; before 220-03.
- [[task-220-03-bp-shield-chaser-wiring]] — bp: BP_ShieldChaser mesh, materials, break event, doc node.
- [[task-220-04-shield-chaser-capture]] — cpp: `-game` capture of run, bash and break.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[art-prompts-shield]] — the shield.
- [[enemy-chaser]] — the shield variant.
