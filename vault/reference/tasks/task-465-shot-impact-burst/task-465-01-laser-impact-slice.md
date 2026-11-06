---
id: task-465-01-laser-impact-slice
description: "Bp: Burst rifle impacts become the green flat cartoon burst of the owner reference (representative slice)"
parent: task-465-shot-impact-burst
kind: task
status: todo
---
# Player shot impact burst: bp

- goal: Burst rifle impacts become the green flat cartoon burst of the owner reference (representative slice)
- agent: bp
- depends_on: task-464-player-shot-colour
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), task-432-player-shot-impact-sparks (vault/reference/tasks/task-432-player-shot-impact-sparks.md), task-464-player-shot-colour (vault/reference/tasks/task-464-player-shot-colour.md), Art/Source/impact-burst/owner-ref-impact-burst-2026-11-05.png, Art/Source/muzzle-flash/owner-ref-green-laser-muzzle-2026-11-05.webp
- expected_output: Expected result: at each burst rifle hit, a bright pale core, a radial star of sharp spikes in several greens, a soft dark green glow and a few small solid and outlined triangles flying out, gone in a fraction of a second, about the size of today's sparks (at most twice). Rules to preserve: flat graphic angular shapes as in the reference, never the Shooter acid green, no gameplay change. Starts after task-464 is done.
- done_when: In a -game run each burst rifle hit on a robot, rock or barrel shows the green cartoon burst described in expected_output instead of the old sparks; robots and damage numbers stay readable during a full volley; shotgun, turret and enemy impacts not yet changed; no gameplay value changed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-465-shot-impact-burst]] — parent global task.
