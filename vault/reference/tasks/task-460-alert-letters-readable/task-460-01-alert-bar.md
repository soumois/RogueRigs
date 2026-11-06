---
id: task-460-01-alert-bar
description: "Bp: The alert bar draws event letters whole in front of horde marks, the marks rising above the bar top."
parent: task-460-alert-letters-readable
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-460-alert-letters-readable]] — parent global task.
