---
id: agent-text
description: "Text agent: writes upgrade texts, barks, alert and console lines from the lore nodes and logs what it retrieved. Invoke for any player-facing text."
parent: agents-index
kind: spec
status: provisional
---
# Text agent

- role: write player-facing text that sounds like Rogue Rigs; never changes a mechanic, a number or a name.
- invoked_by: orchestrator
- trigger: a subtask asking for text content (Assignment #4 first: upgrade texts, barks, alert and console lines).
- reads: its subtask node, [[lore]], [[crew-characters]], [[fictional-films]], [[company-voice]], [[dialogue-tone]], [[readability-rules]], and the one mechanic node the subtask names (for example [[upgrades]] or [[level-flow]]). Nothing else.
- definition_file: `.claude/agents/text.md`.
- produces: one content file in `Content/RogueRigs/Data/Text/` and a retrieval log in `Tools/content/logs/<run-id>.json`.
- output_format: content schema `open` until its pipeline node exists; the log holds, per generated item, the query, the node ids read, the exact excerpt used and the output.
- definition_of_done: the text validator exits 0 (script `open`); [[agent-style-guide]] returns pass on every item, or the owner rules after 2 correction loops.
- must_not: change a number, mechanic or name; invent a value marked `open`; cite a past job or film absent from [[crew-characters]] or [[fictional-films]]; write outside its two paths.
- model: sonnet
- token_ceiling: 30k per task (provisional)

## Relations
- [[agents-index]] — parent index.
- [[agent-executor]] — who delegates to it.
- [[agent-style-guide]] — the critic that reviews its output.
