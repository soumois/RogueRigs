---
id: task-173-debris-count-variety
description: "Global task: throw 2 to 4 debris meshes per death with equal odds and check in a game run that the pieces vary."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Robot debris: 2 to 4 pieces and variety

- request: J'ai l'impression de voir souvent les mêmes pièces (et passe entre 2 et 4 pièces) (owner, 2026-10-14)
- goal: Throw 2 to 4 debris meshes per death with equal odds and check with a game run that the pieces drawn vary, fixing any bias.
- acceptance: Build passes with the editor closed; existing enemy tests still pass except the known EyeMesh one; the frequency table of the 13 meshes and of the counts is reported (values, no screenshots); doc updated; committed by path; committed.
- gdd_check: enemy-death read and updated with the owner values (2026-10-14): 2 to 4 pieces, throw factor 1.3-2 per death, ±15° per piece, start ±10 cm, 3 s then sink 0.4 s without shrink; colour and size left unchanged (owner did not ask).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M3
- next_step: task-173-01-debris-count-variety
- result: open

## Subtasks
1. task-173-01-debris-count-variety — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-173-01-debris-count-variety]] — step 1, cpp.
