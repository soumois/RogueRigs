---
id: task-383-01-total-ore-text
description: "Text: replace the two ore lines of intermission_recap in screen_texts.json by the single Total Ore label."
parent: task-383-level-end-total-ore
kind: task
status: todo
---
# Stats screen Total Ore: text

- goal: In `Content/RogueRigs/Data/Text/screen_texts.json`, item `intermission_recap`: `lines` becomes the single entry of the Total Ore label as decided in the global Q2 (default: "Total Ore:" with no {ORE}, the number is a separate widget text; other choice: "Total Ore: {ORE}"); "Ore secured this level: {ORE}" and "Ore secured this run: {ORE}" removed. Labels and their indices unchanged; no other item touched.
- agent: text
- depends_on: none
- inputs: agent-text (vault/reference/agents/agent-text.md), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), text-content-schema (vault/reference/tech/pipelines/text-content-schema.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md) (reads Lines[0..1] today), Global request and Questions of task-383-level-end-total-ore for the decided wording
- expected_output: screen_texts.json updated with a new valid `run_id`; retrieval log in `Tools/content/logs/`; one commit `task-383-01-total-ore-text: <summary>`.
- done_when: the validator passes; `intermission_recap` lines hold only the Q2 label, labels unchanged (index map in `result`); committed by path.
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
- [[task-383-level-end-total-ore]] — parent global task.
