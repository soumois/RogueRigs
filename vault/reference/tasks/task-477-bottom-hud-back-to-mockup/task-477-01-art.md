---
id: task-477-01-art
description: "Art: Frame pieces for the health strip, block frame, slot cell, XP rail and Lv plate, taken from the mockup per Q1."
parent: task-477-bottom-hud-back-to-mockup
kind: task
status: done
---
# Bottom HUD redone from the art proposal, up to 6 correction rounds: art

- goal: Frame pieces for the health strip, block frame, slot cell, XP rail and Lv plate, taken from the mockup per Q1.
- agent: art
- depends_on: none
- inputs: art-prompts-hud-unified-mockup (vault/reference/gdd/art-audio/art-prompts-hud-unified-mockup.md), task-467-bottom-hud-match-target (vault/reference/tasks/task-467-bottom-hud-match-target.md), Art/Source/hud-unified/20261105-152627-hud-unified-1.png, Saved/Scratch/hud-477/owner-target-crop-2026-11-06-temporary.png
- expected_output: see done_when
- done_when: Each piece matches the mockup in colour, wear, rivets and corner plates (C2, C3) on a side-by-side sheet; sized for 1080p drawing; keep and temporary labels written in art-prompts-hud-unified-mockup; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 10, cache write 28843, cache read 80395, output 4826
- usage: turns 5, context start 10k peak 29k, resumes 0, 1 min, units 0.07 M
- runs: 1
- milestone: M2
- started: 2026-11-06T19:36:24Z
- waiting_for: none
- result: Five pieces cut directly from the mockup scaled to 1920x1080 (Q1, 0 credits) by Art/Source/hud-unified/cut-477/cut.js, at their 1080p pixel size: T_HUD_MockStrip 705x43, T_HUD_MockBlock 739x134, T_HUD_MockCell 163x108, T_HUD_MockRail 1765x62, T_HUD_MockLevelPlate 156x63 (Art/UI/HUD). Side-by-side Saved/Scratch/hud-477/sheet-01-pieces.png: colour, wear, rivets, shoulder plates as the mockup (C2, C3). Critic: art rules pass on all; 2 cleaning fails (strip text specks, rail specks) fixed in the same pass. Labels in art-prompts-hud-unified-mockup. Commit 2178c8c5.

## Relations
- [[task-477-bottom-hud-back-to-mockup]] — parent global task.
