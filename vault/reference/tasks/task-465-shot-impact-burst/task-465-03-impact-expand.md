---
id: task-465-03-impact-expand
description: "Bp: Turret impacts get the green burst sized to the turret shot; shotgun pellet impacts get the same burst in white with warm gold-orange accents"
parent: task-465-shot-impact-burst
kind: task
status: in-progress
---
# Player shot impact burst: bp

- goal: Turret impacts get the green burst sized to the turret shot; shotgun pellet impacts get the same burst in white with warm gold-orange accents
- agent: bp
- depends_on: task-465-02-slice-capture
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), shotgun (vault/reference/gdd/weapons/shotgun.md), Art/Source/impact-burst/owner-ref-impact-burst-2026-11-05.png
- expected_output: see done_when
- done_when: In a -game run turret shot impacts show the green burst of task-465-01 sized to the turret shot; shotgun pellet impacts show the same burst in white with the warm gold-orange accents and glow of the reference; enemy impacts, mining laser and aim laser unchanged; no gameplay value changed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T09:42:43Z
- waiting_for: none
- result: open

## Relations
- [[task-465-shot-impact-burst]] — parent global task.
