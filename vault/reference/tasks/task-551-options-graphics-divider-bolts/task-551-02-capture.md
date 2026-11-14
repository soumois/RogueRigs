---
id: task-551-02-capture
description: "Capture: In-game check of the Options Graphics tab against the pause screen, at the resolutions decided in Q2."
parent: task-551-options-graphics-divider-bolts
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-551
- usage: included in the executor run of task-551
- runs: 4
- milestone: M2
- started: 2026-11-14T08:45:51Z
- waiting_for: none
- result: 4 launches (ui-screens.js pause+options at 1920x1080 with --compare t549_after, 1280x720, 1920x1200, 2560x1080). C1 seen: top bolt centred on the top frame bar at the four resolutions (Saved/Captures/t551_1920x1080/crop_top.png). C2 seen: bottom bolt on the bottom frame bar (crop_bot.png). C3 seen: no floating bolt, bar continuous between them (Saved/Captures/t551_sheet.png). C4 seen: Display and Quality rows unchanged, 100% value clear of the bar at all four, tightest at 1920x1200 (Saved/Captures/t551_pct.png). C5 seen: pause divider unchanged, PSNR 59.9 dB vs t549_after (t551_1920x1080/compare-pause.png). Options PSNR 35.1 dB, change limited to the divider (compare-options.png). Sheet for the owner: Saved/Captures/t551_sheet.png.

## Relations
- [[task-551-options-graphics-divider-bolts]] — parent global task.
