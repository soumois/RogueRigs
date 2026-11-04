---
id: task-426-03-radial-mask-doc
description: "Doc: document the new darkener"
parent: task-426-screen-radial-darkener
kind: task
status: done
---
# Darkener shader: circles around the screen centre, min at centre, max at corners: doc

- goal: Rewrite mpp-player-light-mask (parameters table, formula, tuning, revert to the world-space mask) and note in sys-player that PlayerPosition is no longer read by the shader (per Q10) and that the bDriveTunables names no longer exist; update ppv-xp-glow only if its blendable line changed.
- agent: doc
- depends_on: task-426-01-radial-mask-material
- inputs: mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), sys-player (vault/reference/tech/systems/core/sys-player.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), agent-doc (vault/reference/agents/agent-doc.md)
- expected_output: Doc nodes updated, lint clean, commit.
- done_when: mpp-player-light-mask states the new parameters, formula and revert; sys-player notes the unused PlayerPosition; lint clean; committed.
- editor_state: none
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 12, cache write 22139, cache read 101807, output 4075
- usage: turns 6, context start 11k peak 27k, resumes 0, 1 min, units 0.06 M
- runs: 1
- milestone: M2
- started: 2026-11-04T17:10:09Z
- waiting_for: none
- result: mpp-player-light-mask rewritten (parameters, formula, revert, verification state), sys-player notes unused PlayerPosition; lint clean, commit 7aa33a16.

## Relations
- [[task-426-screen-radial-darkener]] — parent global task.
