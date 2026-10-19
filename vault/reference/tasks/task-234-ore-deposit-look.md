---
id: task-234-ore-deposit-look
description: "Global task: each 2 x 2 ore or heal deposit drawn as one crystal formation, 3 variants per type, from owner images."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Ore deposit look

- request: "le bloc de 2x2 représente une entité ( pour ça qu'on le mine entièrement et qu'il explose si une dalle est dans l'explosion ). Il faut au moins 3 variation chacun ( vie et minerai purple pour l'instant )" ; "Refaire des essaie visuel ( tu me donnes les prompts et je test avec chatgpt pour les avoir les modéles pour la vie et les minerai" (owner, 2026-10-19)
- goal: Each 2 x 2 ore or heal deposit is drawn as one crystal formation, at least 3 variants per type, from images the owner generates with prompts we write.
- acceptance: prompt node written and linted; prompts sent to the owner; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-19).
- confirmed_by_owner: yes (2026-10-19)
- milestone: M2
- next_step: owner images (then 3d, import, capture subtasks)
- result: open

## Subtasks
1. task-234-01-ore-deposit-look — art (prompts)
Later, once the owner sends his images (planned then): Blender models (3d), import and grid assignment per deposit with variants (cpp + bp), capture.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-234-01-ore-deposit-look]] — step 1, art.
