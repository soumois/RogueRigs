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
- [[task-446-explosions-red]] — The whole barrel and kamikaze explosion, sphere, burst and ring, is redone in the cartoon style of the owner reference. Status todo.
- [[task-447-swirls-alpha-fix]] — The rotor wash swirls of task-442 read alpha 0 in the simulation: make them actually show, look values unchanged. Status todo.
- [[task-448-death-puffs-sim-check]] — Read the robot and heavy death puffs in the simulation on real kills, and fix them only if the compiled values are wrong. Status todo.
- [[task-449-level-up-wave-follows]] — The gold level-up floor wave follows the miner while it plays, instead of staying where he levelled up. Status todo.
- [[task-450-remove-hud-level-up-text]] — The cyan LEVEL UP text of the HUD is removed; the gold text in the world stays the only one. Status todo.

