---
id: task-232-drone-single-beam
description: "Global task: The drone shows one laser beam aimed at the middle of the dug blocks, whatever their number."
parent: tasks-done-index
kind: task
status: done
---
# One drone beam

- request: "Il faudrait juste un rayon, même si on mine plusieurs rochers. ( il vise au milieu )" ; défaut accepté : visé au milieu des points d'impact des blocs creusés, sur la face du mur (owner, 2026-10-19)
- goal: The drone shows one laser beam aimed at the middle of the dug blocks, whatever their number.
- acceptance: build exits 0 editor closed; RogueRigs.* tests pass except the 2 known pre-existing failures (tests expecting one beam per block updated); capture sent to the owner; docs updated; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-19).
- confirmed_by_owner: yes (2026-10-19)
- milestone: M2
- next_step: none
- result: Done. Commits 17210e62 (code, tests), 02a18448 (docs). Dig draws one beam (LaserMesh) to GetDigImpactPoint (mean of the dig target points projected on the nearest face; BlueprintPure for 233-02); fan pool removed; hack beam unchanged. Build 0, tests 262/264 (2 known). Captures Saved/DroneBeam/ show one beam, end point hidden by the miner; corner arcs not checked.

## Subtasks
1. task-232-01-drone-single-beam — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-232-01-drone-single-beam]] — step 1, cpp.
