---
id: task-385-01-validator-caption-lines
description: "Cpp: validate_text.js checks a screen_texts lines entry ending with a colon against 1 to 4 words."
parent: task-385-text-caption-lines
kind: task
status: in-progress
---
# Text validator caption lines: validator

- goal: In `Tools/content/validate_text.js` (Node, no dependency): for screen_texts.json only, a `lines` entry whose trimmed text ends with ":" is checked against 1 to 4 words (the labels limit, per global Q1); every other `lines` entry keeps 3 to 12; other files, slots, the uniqueness, digits, T8 and log checks unchanged. The word-count message keeps its form and names the limit applied. Update the header comment if it lists the limits. No new self-test case.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), text-content-files (vault/reference/tech/pipelines/text-content-files.md) (current lines rule), `Tools/content/validate_text.js`, Global request, acceptance and Questions of task-385-text-caption-lines for the decided rule
- expected_output: validate_text.js changed; change summary (where the limit is chosen, the message); one commit `task-385-01-validator-caption-lines: <summary>` with only that file.
- done_when: `node Tools/content/validate_text.js --selftest` exits 0; `node Tools/content/validate_text.js Content/RogueRigs/Data/Text/screen_texts.json --log Tools/content/logs/20261028-170000-screen_texts.json` exits 0 as before; a scratch copy under the scratchpad with intermission_recap lines ["Total Ore:"] gives no word-count error, and one with a 2-word line without ":" still gives `allowed 3 to 12`; committed by path.
- editor_state: any (no build)
- token_ceiling: 0.5 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T05:17:52Z
- waiting_for: none
- result: open

## Relations
- [[task-385-text-caption-lines]] — parent global task.
