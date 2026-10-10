---
id: task-082-01-shuttle-lines
description: "A4 shuttle and extraction lines rewritten so the company states what it does and never orders the player (new rule T9), then reviewed."
parent: task-082-shuttle-lines
kind: task
status: done
---
# A4 shuttle and extraction lines

- goal: A4 shuttle and extraction lines rewritten so the company states what it does and never orders the player (new rule T9), then reviewed.
- agent: text
- depends_on: none
- inputs: assignment-4-open-decisions (vault/reference/course/assignments/assignment-4-open-decisions.md), agent-text (vault/reference/agents/agent-text.md), lore (vault/reference/gdd/lore.md), company-voice (vault/reference/gdd/company-voice.md), dialogue-tone (vault/reference/gdd/art-audio/dialogue-tone.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), text-content-files (vault/reference/tech/pipelines/text-content-files.md), text-content-events (vault/reference/tech/pipelines/text-content-events.md), sys-hud-pressure (vault/reference/tech/systems/sys-hud-pressure.md)
- expected_output: rewrite the 3 shuttle_countdown lines and the 3 extraction_ready lines of Content/RogueRigs/Data/Text/alerts.json (the 9 other lines stay unchanged). Kessler-Voss voice (T6, no jokes, no Rigs jargon), registers mixed as before, 3 to 12 words. New rule T9 in dialogue-tone: the company never orders the player to stop or do something that contradicts the objective (the player must reach the shuttle and survive until it leaves); it states what the company does (reinforcements inbound, recovery units dispatched). The countdown means "company reinforcements arriving" (lore). New run id with suffix alerts, new retrieval log citing the T9 line and the lore mapping; the previous run files stay in logs as history; only a file that passes validate_text.js stays in Content/. The Orchestrator runs the critic next (task-082-02, round 0 then round 1 on corrections). Agent output files are pure JSON or CSV: no prose, no markdown. A failing item never stays in Content/ or in a vault node; failures go to Tools/content/logs/ only.
- done_when: validate_text.js exits 0; 15 lines, 9 unchanged; critic verdict on all 15; no line gives the player an order that contradicts the objective.
- editor_state: any
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: 6 lines rewritten (run 20261010-112240-alerts), validator exit 0, commit efb38e7

## Relations
- [[task-082-shuttle-lines]] — parent global task.
