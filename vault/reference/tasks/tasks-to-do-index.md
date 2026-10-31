---
id: tasks-to-do-index
description: "Open global tasks (in progress, wait-answers, todo, review, blocked), one line each with status. Read on every Orchestrator start."
parent: tasks-index
kind: note
---
# Tasks to do

A global task moves to [[tasks-done-index]] when it is done or cancelled (`vault/scripts/task-status.js` does it). A global waiting for the owner stays here as `Status wait-answers.`; its node's `waiting_for` says what it waits for.

## Relations
- [[tasks-index]] — parent index.
- [[task-314-chest-lid-logo]] — Add the yellow Kessler-Voss mark on top of the chest lid, as on the validated sheet, and show it in game. Status todo.
- [[task-406-miner-upper-body-idle-layer]] — ABP_Miner layers the idle rifle pose on the upper body from Spine over the Mixamo locomotion legs, source swappable later. Status wait-answers.
- [[task-407-miner-right-arm-fix-compare]] — right elbow raised a little and right hand slightly forward on the upper body pose, owner offsets, same-frame comparison images. Status wait-answers.
