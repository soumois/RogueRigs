---
id: task-270-08-chaser-brightness-r4
description: "Chaser body brightness lowered so the yellow chaser no longer clips to white under the player light, set and documented."
parent: task-270-ambience-capture-tuning
kind: task
status: done
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
- model: sonnet
- tokens: input 56, cache write 101943, cache read 864523, output 1085
- runs: 2
- milestone: M1
- result: Commit ed198131. BodyBrightness 0.7 -> 0.5 (master default, read back) on M_Chaser_Meshy_Anim and M_ShieldChaser_Anim; MI_ShieldChaser_Emblem override 1 kept; recompiled and saved; bp-robot-readability updated, revert 0.7. First run killed by an API rate limit before saving; the second run saved the edit still open in the editor.

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
