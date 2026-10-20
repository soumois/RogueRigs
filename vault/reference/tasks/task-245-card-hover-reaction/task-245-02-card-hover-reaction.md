---
id: task-245-02-card-hover-reaction
description: "Capture a hovered card in the game and send the frame."
parent: task-245-card-hover-reaction
kind: task
status: done
---
# 245-02 cpp

- goal: Capture a hovered card in the game and send the frame.
- agent: cpp
- depends_on: task-245-01-card-hover-reaction
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md)
- expected_output: captured frame
- done_when: frame sent to the owner; unseen items listed
- editor_state: closed
- token_ceiling: 40000 (provisional)
- model: open
- tokens: open
- runs: 1
- milestone: M2
- result: Capture -game with a real cursor (SetCursorPos, second monitor window): scale 1.06, tilt direction follows the pointer side (left of card: counter-clockwise, right: clockwise), glow brighter, return to rest, sheen on purple cards and none on grey. Possible defect seen once per jump: one frame of lean toward the exit direction after the cursor leaves (pointer read outside the card while the hover flag is still true; may be a SetCursorPos jump artifact). Unseen: blue/yellow sheen, weapon cards, sheen travel, ease timing, gradual cursor motion. Sent to the owner. 1 run.

## Relations
- [[task-245-card-hover-reaction]] — parent global task.
