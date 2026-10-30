---
id: task-391-05-doc-ore-balance
description: "Doc: update the run, transition, ore counter and stats screen docs: every on-screen ore number is the balance held"
parent: task-391-final-screens-count-level-ore
kind: task
status: todo
---
# Every on-screen ore number is the balance: doc

- goal: Record in the system and widget docs that the stats screen Total Ore, the shop balance, the run-won end screen line and the HUD counter all show the ore the player holds; what the recap field now carries; what task-391-01 (cd19ba03) added and task-391-04 removed or kept; the log lines and the log verification.
- agent: doc
- depends_on: task-391-04-recap-ore-balance
- inputs: agent-doc (vault/reference/agents/agent-doc.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-objectives-ore-counter (vault/reference/tech/systems/world/sys-objectives/sys-objectives-ore-counter.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), commits cd19ba03 and the task-391-04 commit, global request and gdd_check: vault/reference/tasks/task-391-final-screens-count-level-ore.md
- expected_output: The listed doc nodes updated (old wording "ore secured since the run start, purchases not subtracted" and RecordIntermissionOreSpent baselines replaced where they no longer hold); commit `task-391-05-doc-ore-balance: ...`.
- done_when: Every listed node states the balance rule and the current code; no node still says the stats screen shows ore collected since the run start; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-391-final-screens-count-level-ore]] — parent global task.
