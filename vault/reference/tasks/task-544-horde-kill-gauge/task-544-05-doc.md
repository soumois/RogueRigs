---
id: task-544-05-doc
description: "Doc: Document the horde gauge in the system and widget nodes"
parent: task-544-horde-kill-gauge
kind: task
status: done
---
# Hordes: one "!" at half of C, then a kill gauge under the alert bar triggers each horde: doc

- goal: Document the horde gauge in the system and widget nodes
- agent: doc
- depends_on: task-544-04-capture
- inputs: sys-difficulty (vault/reference/tech/systems/core/sys-difficulty.md), level-flow (vault/reference/gdd/run/level-flow.md)
- expected_output: see done_when
- done_when: sys and bp nodes describe the gauge, its overtime multipliers, the health step per overtime horde, its data values and events; the render and art files listed in a vault node; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-544
- usage: included in the executor run of task-544
- runs: 1
- milestone: M2
- started: 2026-11-14T00:50:23Z
- waiting_for: none
- result: Commit 6cec7356: sys-difficulty gains the horde kill gauge bullet (trigger, FRRDirectorKillGauge values, kill forwarding, clear rule, overtime weights and health step, events, debug command, tests); wbp-alert-objectives gains horde_gauge (tree, anchors, offsets, z-order, graphs ShowHordeGauge / RevealHordeGauge / SlideHordeGauge / PulseHordeGauge, hide paths) and lists the render and art files of Art/Source/hud-horde-gauge/. Lint clean. Not edited (executor must_not change a gdd node): level-flow M1 still says the gauge is 'centred under' the bar; the owner addition (attached to the bar, slides out from behind it) is to be written there by the planner or the main session. ALERT: executor over the standard budget (about 5.6 M units, under 4x).

## Relations
- [[task-544-horde-kill-gauge]] — parent global task.
