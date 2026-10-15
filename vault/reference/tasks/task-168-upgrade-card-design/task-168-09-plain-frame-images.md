---
id: task-168-09-plain-frame-images
description: "Use the baked frame textures as plain stretched images in WBP_UpgradeChoice, panel inside the frame only, content in the opening."
parent: task-168-upgrade-card-design
kind: task
status: todo
---
# Upgrade cards: plain frame images

- goal: The cards render like the baked control sheet: thin frame, one dark background inside the frame only, content inside the opening.
- agent: bp
- depends_on: task-168-08-baked-card-frames
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md); the 168-08 report (baked texture paths and opening insets)
- expected_output: The four baked PNGs imported to /Game/RogueRigs/UI/Cards/ (UI group, no mips); FrameBase, FrameAccents, FrameGlow, FrameWeapon use them with Draw As Image (no Box margins), Fill slots over the 440 x 720 card, accents and glow still tinted by tier; any runtime brush change on the frame images (SetBrushFromTexture / MatchSize / SetBrushResourceObject that resets ImageSize) found and removed, tint only at runtime; CardPanel (the 168-06 full-card dark panel) shrunk to the frame opening (padding = opening inset / 2 so its edges hide under the frame) so no second background shows outside the frame; content padding recomputed from the measured opening (inset / 2 + about 10 px) for stat and weapon cards; old 9-slice textures left in place; doc updated.
- done_when: Compiles with warnings as errors, saved, brush draw type and sizes read back at runtime path (the functions that set the frames), doc updated, committed. Owner checks on screen.
- editor_state: open
- token_ceiling: 40 turns
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-168-upgrade-card-design]] — parent global task.
