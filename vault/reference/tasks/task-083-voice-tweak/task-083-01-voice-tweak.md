---
id: task-083-01-voice-tweak
description: "A4 voice tweak: upgrade lines regenerated with an item-specific retrieval and compared with v1, a real tweak for the README."
parent: task-083-voice-tweak
kind: task
status: in-progress
---
# A4 voice tweak (upgrade lines v2)

- goal: A4 voice tweak: upgrade flavor lines regenerated with an item-specific retrieval, compared with the first version, so the README shows a real prompt or retrieval tweak.
- agent: text
- depends_on: none
- inputs: assignment-4-plan (vault/reference/course/assignments/assignment-4-plan.md), assignment-4-open-decisions (vault/reference/course/assignments/assignment-4-open-decisions.md), agent-text (vault/reference/agents/agent-text.md), lore (vault/reference/gdd/lore.md), crew-characters (vault/reference/gdd/crew-characters.md), fictional-films (vault/reference/gdd/fictional-films.md), dialogue-tone (vault/reference/gdd/art-audio/dialogue-tone.md), upgrades (vault/reference/gdd/weapons/upgrades.md), text-content-files (vault/reference/tech/pipelines/text-content-files.md), text-run-log-schema (vault/reference/tech/pipelines/text-run-log-schema.md)
- expected_output: v1 is the current Content/RogueRigs/Data/Text/upgrade_texts.json (run 20261010-103239-upgrade_texts, critic-approved); keep its files in logs. v2: for each of the 13 stats retrieve an item-specific excerpt (its own line in the upgrades node plus the Rook trait that fits that stat from crew-characters, instead of the same two voice excerpts for all 13), log fictional-films as a read node whenever a title is quoted, and write each line to be clearer about what the stat does while keeping Rook voice (T4, T2, T3, T8, T9). Same schema, 3 to 12 words, no digit, stat names unchanged. v2 goes to Content/ only if it passes the validator and the critic (task-083-02); otherwise v1 stays. Record the tweak in the log header field the schema allows or in the run notes of the log, so the README can cite it. No gameplay change. Agent output files are pure JSON or CSV: no prose, no markdown. A failing item never stays in Content/ or in a vault node; failures go to Tools/content/logs/ only.
- done_when: validate_text.js exits 0 on v2; v1 and v2 side by side in the logs; critic verdict on 13 items; the tweak recorded.
- editor_state: any
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-083-voice-tweak]] — parent global task.
