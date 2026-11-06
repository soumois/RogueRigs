---
id: task-460-01-alert-bar
description: "Bp: The alert bar draws event letters whole in front of horde marks, the marks rising above the bar top."
parent: task-460-alert-letters-readable
kind: task
status: done
---
# Alert bar letters never hidden by a horde mark: bp

- goal: The alert bar draws event letters whole in front of horde marks, the marks rising above the bar top.
- agent: bp
- depends_on: none
- inputs: wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), task-453-alert-bar-redesign (vault/reference/tasks/task-453-alert-bar-redesign.md), art-prompts-hud-unified-mockup (vault/reference/gdd/art-audio/art-prompts-hud-unified-mockup.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-05b.webp, Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp, Art/Source/hud-unified/20261105-152627-hud-unified-1.png
- expected_output: see done_when
- done_when: Compiled and saved with no error; at level start in PIE the C under a mark is whole and in front (widget tree or frame read back); no runtime error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-460
- usage: included in the executor run of task-460
- runs: 1
- milestone: M2
- started: 2026-11-06T04:18:54Z
- waiting_for: none
- result: Commit 4eeca4f4. WBP_AlertObjectives: AlertIconConsole, Chest, Shuttle slot ZOrder 3 -> 4 (drawn in front of AlertIconHorde0..3, z 3); AlertIconHorde0..3 slot Top 66 -> 48 (centre on the bar top, sprite 29 x 60 spans y 18..78, top above the frame top 21, inside the screen); letters stay at y 66, X positions (RenderTranslation = Position x 600) and graphs untouched; sprites, colours, fill unchanged. Tree read back, compiled with warnings as errors, saved; PIE L_ArenaM1 started and stopped with no LogScript error or Accessed None. No capture (A6): look unverified until the owner sees it.

## Relations
- [[task-460-alert-letters-readable]] — parent global task.
