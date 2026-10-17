---
id: task-215-01-no-prefix-code
description: "cpp: The subtitle system shows kessler_voss lines without a speaker prefix"
parent: task-215-company-no-prefix
kind: task
status: in-progress
---
# no-prefix-code

- goal: The subtitle system shows kessler_voss lines without a speaker prefix (speaker table entry or per-speaker flag), Rook and Vesna unchanged; update subtitle tests that expect the company prefix.
- agent: cpp
- depends_on: none
- inputs: sys-subtitles (vault/reference/tech/systems/ui/sys-subtitles.md), wbp-subtitles (vault/reference/tech/blueprints/ui/wbp-subtitles.md), company-voice (vault/reference/gdd/company-voice.md), sys-tests-hud-text (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-hud-text.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ (and config default if the speaker table lives there) in Source/RogueRigs/Text/, sys-subtitles updated.
- done_when: build succeeds; tests pass except the known failures (subtitle expectations updated); a -game log shows a company line without prefix; doc updated; committed.
- editor_state: closed
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-215-company-no-prefix]] — parent global task.
