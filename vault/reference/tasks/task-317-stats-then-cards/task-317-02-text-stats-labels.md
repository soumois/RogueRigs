---
id: task-317-02-text-stats-labels
description: "Stats screen and cards screen labels in screen_texts.json."
parent: task-317-stats-then-cards
kind: task
status: todo
---
# Stats screen and cards screen labels in screen_texts.json

- goal: Stats screen and cards screen labels in screen_texts.json. No new test (owner rule): only fix existing tests the change breaks.
- agent: text
- depends_on: task-317-01-stats-then-cards
- inputs: intermission (vault/reference/gdd/run/intermission.md), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), agent-text (vault/reference/agents/agent-text.md)
- expected_output: Content/RogueRigs/Data/screen_texts.json (or the file sys-text-content names): new or updated items for the stats screen (title, run-won title, "Next level" button, headings "Robots destroyed" and "Damage by weapon", run-won button) and the cards screen (title, Continue)
- done_when: items exist in English in the voice rules of agent-text, reviewed by style-guide, existing item keys kept or their renames listed for the bp subtasks; lint clean.
- editor_state: any
- token_ceiling: 20000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-317-stats-then-cards]] — parent global task.
