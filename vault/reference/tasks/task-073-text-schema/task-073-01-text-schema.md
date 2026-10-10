---
id: task-073-01-text-schema
description: "A4 schema: the text-content-schema node defines the 3 content files, the retrieval log and the critic review file."
parent: task-073-text-schema
kind: task
status: done
---
# A4 text-content-schema

- goal: A4 schema: the text-content-schema node defines the 3 content files, the retrieval log and the critic review file.
- agent: cpp
- depends_on: none
- inputs: assignment-4-plan (vault/reference/course/assignments/assignment-4-plan.md), agent-text (vault/reference/agents/agent-text.md), agent-style-guide (vault/reference/agents/agent-style-guide.md), dialogue-tone (vault/reference/gdd/art-audio/dialogue-tone.md), upgrades (vault/reference/gdd/weapons/upgrades.md), sys-upgrades (vault/reference/tech/systems/sys-upgrades.md), sys-hud-pressure (vault/reference/tech/systems/sys-hud-pressure.md), pipelines-index (vault/reference/tech/pipelines/pipelines-index.md)
- expected_output: new node text-content-schema in vault/reference/tech/pipelines/ (template doc), linked from pipelines-index. Defines: file 1 upgrade card texts (id, stat name unchanged, one flavor line), file 2 barks (line_id, speaker_id, trigger, tone from the fixed list cocky, deadpan, technical, propaganda, bureaucratic, text of 8 words at most), file 3 alerts (alert event id, text, register), the retrieval log (per item: query, node ids read, verbatim excerpt, output), the critic review file (item_id, verdict, rule, reason, suggestion). Triggers come from existing events only. Pure JSON, field types and limits listed, one valid example per file.
- done_when: node written with a valid example per file; lint clean; committed.
- editor_state: any
- token_ceiling: 40000
- model: opus
- tokens: input 18, cache write 54704, cache read 263285, output 17821
- runs: 1
- milestone: M2
- result: Schema split in 4 nodes (text-content-schema, -files, -events, text-run-log-schema), commit 308f3ac; 14 bark triggers and 5 alert events from existing events.

## Relations
- [[task-073-text-schema]] — parent global task.
