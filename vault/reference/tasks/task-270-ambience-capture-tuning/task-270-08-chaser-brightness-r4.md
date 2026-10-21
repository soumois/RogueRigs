---
id: task-270-08-chaser-brightness-r4
description: "Chaser body brightness lowered so the yellow chaser no longer clips to white under the player light, set and documented."
parent: task-270-ambience-capture-tuning
kind: task
status: in-progress
---
# Chaser brightness round 4

- goal: Chaser body brightness lowered so the yellow chaser no longer clips to white under the player light, set and documented.
- agent: bp
- depends_on: task-270-06-ambience-capture-r3
- inputs: bp-robot-readability (vault/reference/tech/blueprints/enemies/bp-robot-readability.md), color-code (vault/reference/gdd/art-audio/color-code.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: BodyBrightness on M_Chaser_Meshy_Anim and M_ShieldChaser_Anim set as listed; doc updated with before and after.
- done_when: Values read back; materials compile and are saved; doc updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: 50k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
