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
- [[task-400-burst-rifle-in-hand]] — import SM_BurstRifle, put it on BP_BurstRifle instead of the cube and attach it to a right hand socket of the miner. Status wait-answers.
- [[task-401-miner-hands-fit-rifle]] — the miner left hand holds the rifle handguard by IK in ABP_Miner while the right hand holds the grip, in every clip. Status wait-answers.
- [[task-402-capture-map-path-msys]] — capture.js and Run-StressTest.ps1 pass a correct /Game/... map path even when called from Git Bash (MSYS path rewrite); agent docs updated. Status wait-answers.
