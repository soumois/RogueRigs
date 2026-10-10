---
id: assignment-4-plan
description: "Assignment 4 plan: knowledge base, agents, pipeline steps, paths, proposed content types, what is built. Open before splitting Assignment 4 work."
parent: assignment-4-content-pipeline
kind: note
---
# Assignment 4 — Plan

Decided with the owner on 2026-10-09; open decisions answered on 2026-10-10 ([[assignment-4-open-decisions]]).

## Knowledge base
The lore nodes, an extension of the GDD lore: [[lore]] (L1–L4), [[crew-characters]] (Rook, Vesna), [[fictional-films]], [[company-voice]], [[dialogue-tone]] (T1–T8). No vector database: the agent reads nodes by descent.

## Agents
- Generator: [[agent-text]] (`.claude/agents/text.md`, sonnet). Logs per item the query, the node ids read, the verbatim excerpt used and the output.
- Critic: [[agent-style-guide]] (`.claude/agents/style-guide.md`, opus). Sees the output only, never the generator's prompt. Per item: pass or fail, one rule id, reason, corrected line.
- At most 2 correction loops, then the owner rules.

## Pipeline
Request → text agent (reads lore) → deterministic validator (schema, word counts, numbers unchanged) → critic → text agent applies fails → validator → files in the game → loaded in game.

## Paths
- Content: `Content/RogueRigs/Data/Text/`.
- Logs and reviews: `Tools/content/logs/<run-id>.json` and `<run-id>.review.json`.
- Validator: `Tools/content/validate_text.js`.
- Schema node: `text-content-schema` in `tech/pipelines/`.

## Content types (confirmed by the owner, 2026-10-10)
1. Upgrade card texts: one line per stat. Gap: cards are purely functional ("Damage", "All weapons", "+12%"); `FRRUpgradeCard` already has `DescriptionText` ([[sys-upgrades]]).
2. Barks, Rook and Vesna, per game event. Gap: no dialogue exists. Format from the final GDD: `line_id`, `speaker_id`, `trigger`, `tone` (fixed list of 5), text of 8 words max. Triggers come from existing events ([[sys-hud]], [[sys-objectives]]).
3. Kessler-Voss alerts and announcements per alert event (console, bonus chest, hordes, extraction, shuttle countdown). Gap: the alert bar shows icons, no company voice.

## Film pool (owner 2026-10-10)
Before the runs, [[fictional-films]] grows from 6 to about 50 parody titles of badass 80s and 90s action films (newer ones allowed if they keep the spirit), each recognisable as a parody. The text agent draws from the pool and may leave titles unused. The critic checks the list against T3 (parody titles only).

## Voice judgment (confirmed)
A first run without lore retrieval, compared with the run that reads the vault: the concrete retrieval tweak the rubric asks for.

## README
`assignments/A4-dynamic-content-pipeline/README.md` keeps the vault map markers (`<!-- vault-map:start -->` / `<!-- vault-map:end -->`, filled by `node vault/scripts/vault-map.js`) and the two vault captures in its knowledge base section (owner 2026-10-10).

## Built
Lore nodes, agent specs and definitions, README skeleton.

## Not built yet
`text-content-schema`, `validate_text.js`, the loader and the in-game display, the runs.

## Relations
- [[assignment-4-content-pipeline]] — parent: brief and rubric.
- [[assignment-4-open-decisions]] — decisions this plan waits for.
