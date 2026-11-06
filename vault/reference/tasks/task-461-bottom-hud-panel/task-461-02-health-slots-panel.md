---
id: task-461-02-health-slots-panel
description: "Bp: The health bar and the four weapon slots sit inside one riveted panel."
parent: task-461-bottom-hud-panel
kind: task
status: todo
---
# Bottom HUD built as one riveted panel and a framed XP bar: bp

- goal: The health bar and the four weapon slots sit inside one riveted panel.
- agent: bp
- depends_on: task-461-01-art
- inputs: wbp-weapons (vault/reference/tech/blueprints/ui/wbp-weapons.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), task-452-health-above-weapons (vault/reference/tasks/task-452-health-above-weapons.md), task-458-weapon-slots-style (vault/reference/tasks/task-458-weapon-slots-style.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-05b.webp, Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp
- expected_output: see done_when
- done_when: Compiled and saved with no error; one panel encloses health and slots, no per-slot frames or connectors left; health, slots and card flight still work in PIE with no runtime error.
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
- [[task-461-bottom-hud-panel]] — parent global task.
