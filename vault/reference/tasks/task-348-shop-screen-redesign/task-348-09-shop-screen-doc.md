---
id: task-348-09-shop-screen-doc
description: "Doc: update wbp-shop-card and wbp-shop-screen after the shop screen redesign."
parent: task-348-shop-screen-redesign
kind: task
status: done
---
# Shop screen redesign: update the Blueprint docs

- goal: Update wbp-shop-card and wbp-shop-screen (tree, brushes, labels, new textures, verified) from the commits and change summaries of task-348-05, 06, 07 and the capture of 08; decided values in the global `request` and `## Questions`.
- agent: doc
- depends_on: task-348-05-shop-sprites-import, task-348-06-shop-card-restyle, task-348-07-shop-screen-restyle, task-348-08-shop-screen-capture
- inputs: _authoring (vault/_authoring.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md) (to update), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md) (to update), template `vault/templates/blueprint.md`
- expected_output: the two nodes updated; one commit.
- done_when: nodes match the commits; no value invented; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 10, cache write 31174, cache read 88098, output 5648
- usage: turns 5, context start 12k peak 31k, resumes 0, 1 min, units 0.08 M
- runs: 1
- milestone: M2
- started: 2026-10-28T17:00:08Z
- waiting_for: none
- result: wbp-shop-card and wbp-shop-screen updated (commit 26fcac60) with tree, brushes, labels, textures, verified_redesign and the 5 open layout defects of the capture.

## Relations
- [[task-348-shop-screen-redesign]] — parent global task.
