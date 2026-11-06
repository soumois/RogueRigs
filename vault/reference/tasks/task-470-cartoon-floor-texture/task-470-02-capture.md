---
id: task-470-02-capture
description: "Capture: Check the new floor in game against image 2 and the look before."
parent: task-470-cartoon-floor-texture
kind: task
status: todo
---
# Floor uses the owner cartoon flagstone texture: capture

- goal: Check the new floor in game against image 2 and the look before.
- agent: capture
- depends_on: task-470-01-floor-material
- inputs: agent-capture (vault/reference/agents/agent-capture.md), Art/Source/floor-texture/owner-2026-11-05-floor-cartoon.webp, Art/Source/cartoon-look/reference/owner-2026-11-05-ingame-light-walls.webp, Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: C1 floor shows the image 1 flagstones everywhere with their colours and pink crack rims; C2 flagstone size about that of image 2; C3 no seam or obvious grid repetition; C4 robots, pickups, miner and drone stand out from the floor; C5 ground circles and floor effects still show; side-by-side sheet before / after / image 2 sent to the owner.
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
- [[task-470-cartoon-floor-texture]] — parent global task.
