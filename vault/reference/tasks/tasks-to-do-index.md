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
- [[task-367-extractor-rotors]] — Cut the four rotors of the flattened extractor mesh into separate meshes pivoting on their spin axes, body kept fixed, ready to spin later; runs after task-368. Status in-progress.
- [[task-369-extractor-level-exit]] — Import the extractor body and rotors into Unreal and make the extraction shuttle show it; walking near it validates the level. Status todo.
- [[task-370-extractor-takeoff-exit]] — At the contact the end screen opens, the miner disappears and the extractor takes off like a helicopter behind it and leaves the screen. Status todo.
- [[task-371-extractor-arrival]] — At level start the miner stands beside the extractor, which takes off as soon as the fade in ends. Status todo.
- [[task-372-extractor-clears-rocks]] — When the extractor is set on the ground, blocks under its footprint are destroyed and their ore or heal pickups stay on the floor. Status todo.
- [[task-373-extractor-rotor-spin]] — The four extractor rotors spin on their axes, opposite diagonal pairs, slower on the ground, faster in flight. Status todo.
- [[task-374-extractor-rotor-wash]] — A rotor-wash particle effect plays under each of the four extractor rotors, following the rotor speed. Status todo.
