---
id: task-461-03-xp-frame
description: "Bp: The XP bar sits in its riveted frame with Lv in a framed cap."
parent: task-461-bottom-hud-panel
kind: task
status: in-progress
---
# Bottom HUD built as one riveted panel and a framed XP bar: bp

- goal: The XP bar sits in its riveted frame with Lv in a framed cap.
- agent: bp
- depends_on: task-461-02-health-slots-panel
- inputs: wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md), task-455-xp-bar-ticks (vault/reference/tasks/task-455-xp-bar-ticks.md), task-456-xp-gain-ghost (vault/reference/tasks/task-456-xp-gain-ghost.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp
- expected_output: see done_when
- done_when: Compiled and saved with no error; frame and cap shown, ticks and ghost unchanged in PIE with no runtime error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T04:36:01Z
- waiting_for: none
- result: open

## Relations
- [[task-461-bottom-hud-panel]] — parent global task.
