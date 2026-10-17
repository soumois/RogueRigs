---
id: task-215-company-no-prefix
description: "Global task: Company subtitles without speaker prefix."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Company subtitles without speaker prefix

- request: "fait le 1, puis le 4, puis 7,8,9." (point 9: "Kessler-Voss: Kessler-Voss reinforcements..."), then "Retire le préfixe sur 9" (owner, 2026-10-17)
- goal: Kessler-Voss lines show without the "Kessler-Voss:" prefix, so the four alerts naming the company no longer repeat it.
- acceptance: in game a Kessler-Voss alert shows as the bare line, while Rook and Vesna keep their prefix; capture sent to the owner; tests pass except the known failures; docs updated; lint clean; committed.
- gdd_check: [[company-voice]], [[sys-subtitles]] (speaker prefix "<Speaker>: <line>"). 4 of the 15 alerts name the company. Owner 2026-10-17: remove the prefix for the company; alert texts unchanged, so no text agent and no style-guide review.
- confirmed_by_owner: yes (2026-10-17)
- milestone: M2
- next_step: task-215-02-capture
- result: open

## Subtasks
1. task-215-01-no-prefix-code — cpp
2. task-215-02-capture — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-215-01-no-prefix-code]] — step 1, cpp.
- [[task-215-02-capture]] — step 2, cpp.
