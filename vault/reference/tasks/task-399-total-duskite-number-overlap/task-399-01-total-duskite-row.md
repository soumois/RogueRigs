---
id: task-399-01-total-duskite-row
description: "Bp: WBP_Intermission: lay out OreIcon, OreLabel and OreValue so the number sits right of the label (Q1)"
parent: task-399-total-duskite-number-overlap
kind: task
status: in-progress
---
# Stats screen: Total Duskite number overlaps its label: bp

- goal: In WBP_Intermission read OreIcon, OreLabel, OreValue (anchors, alignment, offsets, size mode, fonts) and report the state before. Apply the Q1 decision written in the global request/Questions: (A, default) wrap OreIcon, OreLabel and OreValue, in that order, in one HorizontalBox named OreRow, a direct child of Root anchored at (0.5,0.31), alignment (0.5,0.5), auto size; slots vertical align center, horizontal align left, no fill; OreIcon keeps 88 x 88, right padding 12 on OreIcon, left padding 16 on OreValue; keep the three widget names and their variables (the graph sets OreLabel and OreValue); or (B) keep the separate anchors and move OreValue right of the measured end of "Total Duskite:" at 32 with a 16 gap. Fonts (label 32, number 48, outline 2), texts, bindings and every other widget unchanged; the band stays above DividerH. Report the values after.
- agent: bp
- depends_on: none
- inputs: agent-bp (vault/reference/agents/agent-bp.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), intermission (vault/reference/gdd/run/intermission.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), global request, gdd_check and Questions: vault/reference/tasks/task-399-total-duskite-number-overlap.md, owner screenshot: `Saved/LevelEndScreen/task-399/owner-bug-before.webp`
- expected_output: WBP_Intermission saved, compiled with warnings as errors; commit `task-399-01-total-duskite-row: ...`; change summary (values before, values after, tree change) for the doc subtask.
- done_when: Read back: OreIcon, OreLabel, OreValue laid out per the Q1 decision with the values above; names and variables unchanged; OnRecapShown still sets OreLabel and OreValue (graph unchanged); no other widget or property changed; compiled with warnings as errors, no error; saved; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T23:32:52Z
- waiting_for: none
- result: open

## Relations
- [[task-399-total-duskite-number-overlap]] — parent global task.
