---
id: task-275-01-monday-light-revert
description: "DirectionalLight_0 and SkyLight_0 packages of L_ArenaM1 restored from Monday commit 75610b21 (sun 6 white, sky 0.3 white, shadows on); docs updated."
parent: task-275-monday-light-revert
kind: task
status: done
---
# Map lights back to Monday

- goal: DirectionalLight_0 and SkyLight_0 exactly as on Monday by restoring their external actor packages from commit 75610b21 (the parent of task-261-01 45c5d796); only lighting tasks touched these two files since.
- agent: bp
- depends_on: none
- inputs: lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), visual-style (vault/reference/gdd/art-audio/visual-style.md); files Content/__ExternalActors__/RogueRigs/Maps/L_ArenaM1/7/NT/V5ATLGD6KXMEQFO2WZJWIB.uasset (DirectionalLight_0) and Content/__ExternalActors__/RogueRigs/Maps/L_ArenaM1/E/R7/FZQSIBGXQNH4S110A89HHQ.uasset (SkyLight_0).
- expected_output: the two packages restored with `git checkout 75610b21 -- <path>`; lights-dark-ambient and lights-no-shadows updated (before -> after, revert = HEAD values before this task); A12 suspension lines removed from art-style-guide (line under A12 and its Relations gloss) and visual-style (provisional task-261 line and its Relations gloss).
- done_when: `git diff 75610b21 HEAD -- <two packages>` empty after the commit; docs record sun 4.0 (0.75, 0.62, 1.0) castShadows false -> 6 (1, 1, 1) castShadows true, sky 0.08 (0.35, 0.25, 0.6) castShadows false -> 0.3 (1, 1, 1) castShadows true, revert values = the task-272-18 state; editor read-back deferred to task-275-04; lint clean; committed by path.
- editor_state: closed
- token_ceiling: 25k
- model: sonnet
- tokens: input 10, cache write 27752, cache read 69995, output 78
- runs: 1
- milestone: M1
- result: Commit 8c988ff9: DirectionalLight_0 and SkyLight_0 packages restored from 75610b21 (sun 4.0 violet no shadows -> 6 white shadows; sky 0.08 violet no shadows -> 0.3 white shadows); lights-dark-ambient, lights-no-shadows, art-style-guide A12 and visual-style updated; editor read-back in task-275-04.

## Relations
- [[task-275-monday-light-revert]] — parent global task.
