---
id: task-528-extractor-takeoff-under-floor
description: "Global task: Defect: followed after the drop-off, the extractor goes into the floor during its takeoff."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# L'extracteur rentre dans le sol en partant

- request: Owner 2026-11-13, play-test batch in PIE (12 points, family gros morceau split one point per global), with two screenshots (image 1 gameplay: HUD, alert bar C / B / ! / S, a chest light glow with no chest; image 2 Options, Sound tab, the "Sound" tab button drawn smaller than the other tabs). This point, verbatim: « Si je suis le vaisseau lorsqu'il me dépose, je le vois rentré dans le sol, il faut corriger ça. »
- goal: The extractor never goes below the floor during a takeoff, arrival and extraction alike, per Q15.
- acceptance: A1 on arrival, the miner following the extractor sees it climb and leave without any part going below the floor; A2 same for the extraction takeoff; A3 rise, tilt, rotor speeds, wash and blast otherwise unchanged; A4 build green, existing tests pass; capture and doc done.
- gdd_check: Read extraction, level-flow. Rule today: rise 400 uu, then nose down 20 deg and acceleration along the nose, which points downward: conflicts with "never into the floor"; the flight path is asked (Q15). Edit: extraction. No code read.
- verification_level: standard
- family: comportement
- confirmed_by_owner: no (plan and questions handed back 2026-11-13)
- milestone: M2
- next_step: task-528-01-takeoff-path
- started: open
- waiting_for: owner answers Q1 to Q15 and confirmation
- result: open

## Questions
Asked 2026-11-13, each with a default; open until the owner answers.
- Q15 Décollage (arrivée et extraction) : il monte de 400 uu comme aujourd'hui, garde son inclinaison nez en bas de 20°, puis avance selon la direction horizontale de son nez en continuant de monter un peu, sans jamais redescendre, jusqu'à sortir de la vue. Default: oui.

## Subtasks
1. task-528-01-takeoff-path (vault/reference/tasks/task-528-extractor-takeoff-under-floor/task-528-01-takeoff-path.md) — cpp: takeoff path never below the floor per Q15
2. task-528-02-capture (vault/reference/tasks/task-528-extractor-takeoff-under-floor/task-528-02-capture.md) — capture: arrival takeoff followed by the miner
3. task-528-03-doc (vault/reference/tasks/task-528-extractor-takeoff-under-floor/task-528-03-doc.md) — doc: bp-extraction-shuttle: takeoff path

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-528-01-takeoff-path]] — step 1, cpp.
- [[task-528-02-capture]] — step 2, capture.
- [[task-528-03-doc]] — step 3, doc.
