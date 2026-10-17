---
id: task-222-shield-break-debris
description: "When the shield chaser's shield breaks, throw shield fragments as debris, like the robot parts on death."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Shield chaser: shield break debris

- request: "Il faudrait une particule lorsqu'on détruit le bouclier (comme les pièces des robots, mais cette fois-ci c'est le bouclier)." (owner 2026-10-17)
- goal: on OnShieldBroken the shield vanishes (task-220) and its fragments fly out and fall, like [[bp-robot-debris]].
- acceptance: a `-game` capture of a shield breaking with fragments, sent to the owner; doc nodes for the new Niagara system and BP change.
- gdd_check: [[enemy-death]] (robot debris look: 2 to 4 pieces, gravity, bounce, sink after 4.5 s), [[enemy-chaser]] (shield variant), [[art-prompts-shield]]. Defaults (Orchestrator): 5 fragments of the scaled shield (98 cm): four plate shards (the emblem shard among them) and the grip; all thrown each time; same life, gravity, bounce, collision and sink as NS_RobotDebris; thrown away from the robot front with a random spread (C++ hit direction is not reachable from Blueprint, no C++ change); spawned without pool (once per shield chaser).
- confirmed_by_owner: yes
- milestone: M1
- next_step: task-222-02-shield-debris-import
- result: open

## Subtasks
- [[task-222-01-shield-debris-pieces]] — 3D agent: cut the shield into 5 debris meshes with offsets.
- [[task-222-02-shield-debris-import]] — bp: import the pieces and their material.
- [[task-222-03-ns-shield-debris]] — bp: Niagara system NS_ShieldDebris.
- [[task-222-04-bp-shield-chaser-debris]] — bp: BP_ShieldChaser spawns it on OnShieldBroken.
- [[task-222-05-shield-debris-capture]] — cpp: `-game` capture of a shield break.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[bp-robot-debris]] — the debris system copied.
- [[art-prompts-shield]] — the shield.
