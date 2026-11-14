---
id: task-551-02-capture
description: "Capture: In-game check of the Options Graphics tab against the pause screen, at the resolutions decided in Q2."
parent: task-551-options-graphics-divider-bolts
kind: task
status: todo
---
# Options Graphics divider: bolts on the frame borders like the pause screen: capture

- goal: In-game check of the Options Graphics tab against the pause screen, at the resolutions decided in Q2.
- agent: capture
- depends_on: task-551-01-bp-divider
- inputs: agent-capture (vault/reference/agents/agent-capture.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), C:\Users\soumo\AppData\Local\Temp\claude\5598e601faa1cb4a\a568478d-54ab-4716-acd8-b83c02a25a8f\images\1.webp, C:\Users\soumo\AppData\Local\Temp\claude\5598e601faa1cb4a\a568478d-54ab-4716-acd8-b83c02a25a8f\images\2.webp
- expected_output: see done_when
- done_when: Frames of Options Graphics tab, before and after, show: C1 top bolt centred on the top frame border; C2 bottom bolt centred on the bottom frame border; C3 no bolt floating inside the panel, bar continuous between them; C4 Display and Quality rows and controls in place, nothing overlapping the bar; C5 one pause-screen frame shows its divider unchanged; sheet sent to the owner. No other captures.
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
- [[task-551-options-graphics-divider-bolts]] — parent global task.
