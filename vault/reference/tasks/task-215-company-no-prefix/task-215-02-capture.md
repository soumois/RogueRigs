---
id: task-215-02-capture
description: "cpp: In-game capture of a Kessler-Voss alert subtitle without prefix, compared with the task-209-05 capture, sent to the owner."
parent: task-215-company-no-prefix
kind: task
status: done
---
# capture

- goal: In-game capture of a Kessler-Voss alert subtitle without prefix, compared with the task-209-05 capture, sent to the owner.
- agent: cpp
- depends_on: task-215-01-no-prefix-code
- inputs: sys-subtitles (vault/reference/tech/systems/ui/sys-subtitles.md), wbp-subtitles (vault/reference/tech/blueprints/ui/wbp-subtitles.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: PNG capture in Saved/Captures/ listed in the report.
- done_when: the capture shows a company line with no "Kessler-Voss:" prefix; the orchestrator sends it to the owner.
- editor_state: closed
- token_ceiling: 30000
- model: opus
- tokens: input 18, cache write 25010, cache read 148124, output 559
- runs: 1
- milestone: M2
- result: Rendered -game 1920 x 1080 capture sent to the owner: the console_dropped company alert shows as the bare line 'Access point flagged for immediate asset recovery.' with no prefix (WBP_Subtitles uses the event's Subtitle argument). Saved/Captures/task215_alert_*.png.

## Relations
- [[task-215-company-no-prefix]] — parent global task.
