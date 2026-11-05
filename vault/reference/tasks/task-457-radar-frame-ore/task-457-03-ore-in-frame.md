---
id: task-457-03-ore-in-frame
description: "Bp: The ore count shows in the part of the radar frame that extends to take it in; the floating counter is gone."
parent: task-457-radar-frame-ore
kind: task
status: todo
---
# Radar frame holding the ore count: bp

- goal: The ore count shows in the part of the radar frame that extends to take it in; the floating counter is gone.
- agent: bp
- depends_on: task-457-02-radar-frame
- inputs: wbp-ore-counter (vault/reference/tech/blueprints/ui/wbp-ore-counter.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), sys-objectives-ore-counter (vault/reference/tech/systems/world/sys-objectives/sys-objectives-ore-counter.md), umg-layout (vault/reference/tech/conventions/umg-layout.md)
- expected_output: see done_when
- done_when: Compiled and saved; count updates on pickup in PIE; no runtime error.
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
- [[task-457-radar-frame-ore]] — parent global task.
