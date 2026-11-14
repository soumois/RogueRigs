---
id: task-546-01-stats-layout
description: "Bp: Adjust the stats screen frame dividers and bolts to image 6 with the shared pieces"
parent: task-546-level-cleared-look
kind: task
status: done
---
# Level cleared screen: dividers and bolts as owner image 6: bp

- goal: Adjust the stats screen frame dividers and bolts to image 6 with the shared pieces
- agent: bp
- depends_on: none
- inputs: intermission (vault/reference/gdd/run/intermission.md), art-prompts-level-end-screen-mockup (vault/reference/gdd/art-audio/art-prompts-level-end-screen-mockup.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Saved/Scratch/owner-ui-batch-2026-11-13/5.webp, Saved/Scratch/owner-ui-batch-2026-11-13/6.webp
- expected_output: see done_when
- done_when: A1 to A4 hold.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-546
- usage: included in the executor run of task-546
- runs: 1
- milestone: M2
- started: 2026-11-14T03:52:08Z
- waiting_for: none
- result: WBP_Intermission (7e8b0309): DividerH and DividerV images replaced by WBP_UI_BoltedBarH (x 0.22-0.78 insets 30, y point 0.3596, 34 high) and WBP_UI_BoltedBarV (x point 0.5, y 0.3596-0.799, top 0, bottom inset 25, 34 wide), same names, V above H; its top bolt is the junction mid H divider, bottom bolt on the bottom frame bar, H end bolts on the frame sides. Graph untouched (no divider reference). Seen -game 1920x1080 (Saved/Captures/t546_r1_1080/st_00.png): bolts round. Bars and bolts keep the pause sizes (16/34), thinner than image 6, per Q12 same pieces as the pause screen.

## Relations
- [[task-546-level-cleared-look]] — parent global task.
