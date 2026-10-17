---
id: task-211-02-end-hud-bp
description: "bp: On the end-of-run event of task-211-01, WBP_HUD and WBP_AlertObjectives hide every element except the end screen"
parent: task-211-end-screens-clean
kind: task
status: done
---
# end-hud-bp

- goal: On the end-of-run event of task-211-01, WBP_HUD and WBP_AlertObjectives hide every element except the end screen (title, subtitle, ore line, Play again): minimap, health, ore counter, alert bar and icons, objective and shuttle texts, weapons, XP bar.
- agent: bp
- depends_on: task-211-01-end-silence-code
- inputs: wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), hud-refactor-audit (vault/reference/tech/blueprints/ui/hud-refactor-audit.md), agent-bp (vault/reference/agents/agent-bp.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: Content/RogueRigs/UI/WBP_HUD and WBP_AlertObjectives, docs wbp-hud and wbp-alert-objectives updated.
- done_when: widgets compile and save; docs updated; committed.
- editor_state: open
- token_ceiling: 40000
- model: sonnet
- tokens: input 112, cache write 94526, cache read 3471023, output 848
- runs: 1
- milestone: M2
- result: WBP_HUD OnEndScreenShown collapses MinimapLocator, OreCounter, Weapons, XPStatus, Subtitles, UpgradeChoice and HealthStatus (except on death); WBP_AlertObjectives HideForEnd keeps only EndScreen; WBP_HealthStatus keeps GAME OVER and hides its bar, text and vignette on death (29162564). Unseen on screen.

## Relations
- [[task-211-end-screens-clean]] — parent global task.
