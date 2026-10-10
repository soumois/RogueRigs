---
id: text-run-log-schema
description: "Retrieval log (<run-id>.json) and critic review (<run-id>.review.json) formats: fields, verbatim excerpt check, correction loops, examples."
parent: text-content-schema
kind: doc
---
# Text run log and review

Paths, run id and text rules: [[text-content-schema]]. Pure JSON, exact keys, examples reference only (T8).

## Retrieval log `Tools/content/logs/<run-id>.json` (written by [[agent-text]])
- Top level: `run_id` (string), `content_file` (`upgrade_texts.json`, `barks.json` or `alerts.json`; must match the run id stem), `retrieval` (boolean: false exactly when the run id ends `-noretrieval`), `entries` (non-empty array).
- Entry: `item_id` (string), `loop` (integer 0, 1 or 2; 0 = first generation, 1–2 = corrections), `query` (non-empty string: what the agent looked for), `node_ids` (array of unique vault ids, each present in `vault/_paths.md`), `excerpts` (array of `{node_id, text}`), `output` (string).
- Excerpt: `node_id` is listed in `node_ids`; `text` has at least 3 words and appears verbatim, character for character, in that node file (after CRLF → LF; no other normalization).
- `retrieval` true: `node_ids` and `excerpts` non-empty. `retrieval` false: both empty arrays.
- Consistency: (`item_id`, `loop`) unique; every content item has a loop-0 entry; no entry for an id absent from the content file; the highest-loop entry of an item has `output` equal to its content text (`flavor` or `text`); a correction entry exists only for an item failed in the previous review round.

```json
{"run_id": "20261011-150000-barks", "content_file": "barks.json", "retrieval": true, "entries": [
  {"item_id": "vesna_console_dropped_01", "loop": 0, "query": "Vesna voice for a console drop", "node_ids": ["crew-characters"],
   "excerpts": [{"node_id": "crew-characters", "text": "Calm, precise, mission first; short dry sentences."}], "output": "Console is down. Walk, do not sightsee."}]}
```

## Critic review `Tools/content/logs/<run-id>.review.json` (written by [[agent-style-guide]])
- Top level: `run_id` (same as the log), `rounds` (array; round `loop` values 0, 1, 2 in order, at most 3: 2 correction loops, then the owner rules).
- Round: `loop` (integer), `reviews` (array, exactly one per item of the content file at that loop).
- Review: `item_id`, `verdict` (`pass` or `fail`), `rule`, `reason`, `suggestion`.
  - `pass`: `rule` null, `reason` string (may be empty), `suggestion` null.
  - `fail`: `rule` an existing rule id (L1–L4 of [[lore]], T1–T9 of [[dialogue-tone]]), `reason` one non-empty sentence, `suggestion` a string obeying the text rules and word limit of that file ([[text-content-files]]).

```json
{"run_id": "20261011-150000-barks", "rounds": [{"loop": 0, "reviews": [
  {"item_id": "vesna_console_dropped_01", "verdict": "pass", "rule": null, "reason": "", "suggestion": null}]}]}
```

## Relations
- [[text-content-schema]] — parent: the pipeline contract.
- [[text-content-files]] — the items logged and reviewed.
- [[agent-text]] — writes the log.
- [[agent-style-guide]] — writes the review.
