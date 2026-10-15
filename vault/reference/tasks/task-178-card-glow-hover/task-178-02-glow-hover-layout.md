---
id: task-178-02-glow-hover-layout
description: "Show the new glow at about 2x at rest and about 3x on hover of a card, through the card button hover events, no tick."
parent: task-178-card-glow-hover
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-178-card-glow-hover]] — parent global task.
