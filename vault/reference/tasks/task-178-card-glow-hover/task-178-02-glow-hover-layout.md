---
id: task-178-02-glow-hover-layout
description: "Show the new glow at about 2x at rest and about 3x on hover of a card, through the card button hover events, no tick."
parent: task-178-card-glow-hover
kind: task
status: done
---
# Upgrade cards: glow strength and hover

- goal: Show the new glow at about 2x at rest and about 3x on hover of a card, through the card button hover events, no tick.
- agent: bp
- depends_on: task-178-01-card-glow-hover, task-177-02-wider-cards-layout
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md); report of task-178-01-card-glow-hover, task-177-02-wider-cards-layout
- expected_output: Re-imported glow texture; FrameGlow opacity at rest and on hover per the owner-validated sheet; CardButton OnHovered / OnUnhovered bound in Construct to custom events setting the glow opacity of that card (no tick); stat cards only (weapon frame has no glow); doc updated.
- done_when: Compiles, saved, graphs read back, doc updated, committed. Owner checks on screen.
- editor_state: open
- token_ceiling: 40 turns
- model: sonnet
- tokens: input 66, cache write 102913, cache read 1892725, output 450
- runs: 1
- milestone: M2
- result: New glow re-imported (1000 x 1440), FrameGlow0..2 brushes re-assigned; RenderOpacity 0.6 at rest; Construct binds OnHovered / OnUnhovered of CardButton0..2 to events setting FrameGlow<i> opacity 1.0 / 0.6 (no tick); OnClicked handlers rebuilt (OnClicked_Event_7.._10, indices 0/1/2 and reroll fixed with set_pin_value; strays deleted). Compiled, saved, read back, doc updated. Not seen on screen. Commit 9de50ed.

## Relations
- [[task-178-card-glow-hover]] — parent global task.
