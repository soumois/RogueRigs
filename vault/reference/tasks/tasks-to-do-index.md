---
id: tasks-to-do-index
description: "Open global tasks (in progress, todo, review, blocked), one line each with status. Read on every Orchestrator start."
parent: tasks-index
kind: note
---
# Tasks to do

A global task moves to [[tasks-done-index]] when it is done (`vault/scripts/task-status.js` does it).

## Relations
- [[tasks-index]] — parent index.
- [[task-109-weapon-slot-icons]] — Active weapon slots show the weapon icon large without its name, like the reference game; the icon tool keeps the icon aspect ratio. Status in-progress.
- [[task-110-xp-big-block]] — When many XP cubes fall in one place they become one bigger block of another colour worth the same total XP. Status todo.
- [[task-111-pool-prewarm]] — The pools are prewarmed at level start, spread over frames per class (XP 30 over 20, chasers 30 over 20, other robots 5 or fewer over 60), and projectile pooling is evaluated. Status todo.
- [[task-112-ground-telegraph]] — A generic ground signal component draws a zone circle and a growing circle on the floor, dies with its owner, and ignores robots, the miner and player weapons. Status todo.
- [[task-113-capture-zone-visual]] — The console and chest hack zones show a sharp zone circle and a growing progress circle; the old diffuse circle is gone; the rules are unchanged. Status todo.
- [[task-114-barrel-fuse]] — Barrels have a doubled explosion radius and explode 1.5 s after reaching 0 health, with an orange danger zone and a growing circle. Status todo.
- [[task-115-heavy-slam-telegraph]] — The heavy slam shows its danger zone and a growing circle on the ground during the telegraph, and the signal dies with the robot. Status todo.
