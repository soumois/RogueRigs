---
id: task-317-04-bp-stats-screen
description: "WBP_Intermission becomes the stats screen: recap, kills per robot type, damage per weapon, Next level button; shop removed."
parent: task-317-stats-then-cards
kind: task
status: done
---
# WBP_Intermission becomes the stats screen: recap, kills per robot type, damage per weapon, Next level button; shop removed

- goal: WBP_Intermission becomes the stats screen: recap, kills per robot type, damage per weapon, Next level button; shop removed. No new test (owner rule): only fix existing tests the change breaks.
- agent: bp
- depends_on: task-317-01-stats-then-cards, task-317-02-text-stats-labels, task-317-03-bp-shop-screen
- inputs: wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run-log (vault/reference/tech/systems/world/sys-run-log.md), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md)
- expected_output: Content/RogueRigs/UI/WBP_Intermission updated; doc wbp-intermission updated
- done_when: compiled with warnings as errors and saved; shop panel and its cards removed; recap texts kept; a list of robot type names with kill counts and a list of weapon names with damage dealt filled from the recap struct; button "Next level" calls NextLevel(), on the run-won stats it leads to the run-won screen with its own label; backdrop unchanged (over the paused level); fits 1920 x 1080; doc updated; lint clean.
- editor_state: open
- token_ceiling: 40000
- model: sonnet
- tokens: input 92, cache write 100626, cache read 2987605, output 31954
- runs: 1
- milestone: M2
- result: WBP_Intermission is the stats screen: shop panel removed, recap kept, Robots destroyed and Damage by weapon lists from KillsByRobotType and DamageByWeapon, button Next level -> NextLevel(), run-won variant CLAIM EMPTIED / Cash out; backdrop 0.85 unchanged; not run in PIE. Commit 114c7539.

## Relations
- [[task-317-stats-then-cards]] — parent global task.
