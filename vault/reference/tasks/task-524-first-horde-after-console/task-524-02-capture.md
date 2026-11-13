---
id: task-524-02-capture
description: "Capture: alert bar around the console landing and first horde"
parent: task-524-first-horde-after-console
kind: task
status: done
---
# Première horde un peu après l'arrivée de la console: capture

- goal: alert bar around the console landing and first horde
- agent: capture
- depends_on: task-524-01-horde-timing
- inputs: agent-capture (vault/reference/agents/agent-capture.md), level-flow (vault/reference/gdd/run/level-flow.md)
- expected_output: see done_when
- done_when: C1 the first "!" after the C letter, C whole and in front; C2 banner frame after the console landing; frames sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-524
- usage: included in the executor run of task-524
- runs: 1
- milestone: M2
- started: 2026-11-13T13:58:01Z
- waiting_for: none
- result: Done. One -game run (L_ArenaM1, 1600x900, pacing bot, invulnerable; capture launched twice: the first ended before the banner), 12 shots from 104 s. C1 seen: the first horde '!' sits after the C letter, C whole and in front, from before the landing to the banner (Saved/Captures/task524-horde/bar_sheet.png, frames hz_00, hz_04, hz_10, hz_11). C2 seen: 'HORDE INCOMING!' banner on hz_11, taken after the console landing (log: console dropped 96.4 s, banner 112.6 s, front 116.5 s). Frames for the owner: Saved/Captures/task524-horde/contact_sheet.png and bar_sheet.png. Note (unchanged behaviour, not in scope): the second '!' slides back toward the paused fill while the console is pending, toward the B letter.

## Relations
- [[task-524-first-horde-after-console]] — parent global task.
