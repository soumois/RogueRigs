---
id: task-072-01-film-pool
description: "A4 film pool: fictional-films grows to about 50 parody titles of badass 80s and 90s action films, checked by the critic."
parent: task-072-film-pool
kind: task
status: done
---
# A4 film pool

- goal: A4 film pool: fictional-films grows to about 50 parody titles of badass 80s and 90s action films, checked by the critic.
- agent: text
- depends_on: none
- inputs: assignment-4-open-decisions (vault/reference/course/assignments/assignment-4-open-decisions.md), assignment-4-plan (vault/reference/course/assignments/assignment-4-plan.md), fictional-films (vault/reference/gdd/fictional-films.md), crew-characters (vault/reference/gdd/crew-characters.md), dialogue-tone (vault/reference/gdd/art-audio/dialogue-tone.md), agent-text (vault/reference/agents/agent-text.md)
- expected_output: extend the Catalogue of the fictional-films node to about 50 titles (keep the 6 existing, mark new ones provisional): parodies of badass 80s and 90s action films, newer ones allowed if they keep the spirit, each recognisable as a parody by its title alone, no real title unchanged, no tagline or line from a real film (T3). One short title per line, no description. Exception to the usual path rule: this subtask may edit that one vault node. Then the critic (task-072-02) reviews the list; the owner reads it at the end.
- done_when: node has about 50 titles; lint clean; critic verdict recorded; no real line or tagline; committed.
- editor_state: any
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: 50 titles in fictional-films (44 new, provisional), commit eca35ee

## Relations
- [[task-072-film-pool]] — parent global task.
