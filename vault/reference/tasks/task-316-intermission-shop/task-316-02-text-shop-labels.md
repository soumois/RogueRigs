---
id: task-316-02-text-shop-labels
description: "Shop labels of the intermission screen in screen_texts.json (text): upgrade shop between levels, step 02."
parent: task-316-intermission-shop
kind: task
status: todo
---
# Shop labels

- goal: In Content/RogueRigs/Data/Text/screen_texts.json, entry intermission_recap: replace the label "Shop opens later" with a shop title and add the short shop labels the widget needs (title, buy, bought, not enough ore, reroll), English, in the company and crew voice; keep the existing keys, order and the Continue label; reviewed by the style loop.
- agent: text
- depends_on: task-316-01-intermission-shop
- inputs: agent-text (vault/reference/agents/agent-text.md), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), run-shop (vault/reference/gdd/run/run-shop.md); files: Content/RogueRigs/Data/Text/screen_texts.json
- expected_output: screen_texts.json updated, generation log in Tools/content/logs/, label index list in the report (Labels[i] -> meaning) for the bp subtask
- done_when: JSON valid; existing keys and Labels[1] Continue unchanged in position or the report states the new indexes; style loop passed; lint clean; committed
- editor_state: any
- token_ceiling: 20000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-316-intermission-shop]] — parent global task.
