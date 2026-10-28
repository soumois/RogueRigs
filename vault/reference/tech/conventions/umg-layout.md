---
id: umg-layout
description: "UMG rule for bp agents: Root Canvas and anchors first, no fixed pixel sizes, no Row to centre on a font. Read before widget work."
parent: conventions-index
kind: rule
applies_to: UMG widgets (WBP_*) built or changed by agents
---
# UMG layout: Canvas and anchors first

Owner 2026-10-28 ("OK, tout par défaut" to Q5 of [[task-356-shop-screen-anchors]]), provisional. Goal: widgets adapt to small changes of resolution and ratio, and the owner can retouch them by hand in the Designer.

## Rules
- Root is a CanvasPanel. Each top-level block is a direct child with explicit anchors, alignment and offsets: full-screen backgrounds stretch (0,0)-(1,1) with offsets 0; a block tied to an edge or corner is anchored there (top centre (0.5,0), top left (0,0), bottom centre (0.5,1)...) with the matching alignment.
- Sizes come from anchors (min/max fractions of the parent) or from content and padding, not from fixed pixel SizeBox, WidthOverride or forced brush ImageSize. Exception: images shown at their native size.
- Offsets stay small and are slate units (DPI-scaled); the font scale comes from the project DPI rule, never from a hand-set per-resolution value.
- Do not use a HorizontalBox or VerticalBox (Row/Column) to centre elements on a text line; anchor the text and the element to the same point with alignments that put them on the same centre line, and tie the element size to the font size.
- 9-slice plates (Box brushes) take the size of their slot.
- Keep nesting shallow (three levels per block where possible) and give every widget a clear name; never rename a widget a graph uses.
- Start from the asset as it is: if the owner hand-edited it and the change is not committed, commit it alone and unchanged first, with a message saying it is the owner's edits.
- Verify with -game captures at 1920 x 1080 and at least two other resolutions or ratios (default set: 1280 x 720, 1920 x 1200, 2560 x 1080).
- The widget's doc node lists, for each top-level widget, its anchors min/max, alignment, offsets and size mode, so the owner can edit by hand.

## Relations
- [[conventions-index]] — parent index.
- [[agent-bp]] — reads this rule before any widget work.
- [[task-356-shop-screen-anchors]] — the task that set the rule and applies it first.
