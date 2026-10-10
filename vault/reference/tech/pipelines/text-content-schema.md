---
id: text-content-schema
description: "Text pipeline contract: paths, run-id, JSON envelope and text rules of the 3 content files; children hold item fields, events, log and review."
parent: pipelines-index
kind: doc
---
# Text content schema

Contract between the text agent, the validator and the critic of [[assignment-4-plan]]. Every rule here is machine-checkable; a file that breaks one fails.

- responsibility: format of the player-facing text files, the retrieval log and the critic review.
- main_classes: the in-game loader `URRTextContentSubsystem` ([[sys-text-content]]).
- entry_points: `node Tools/content/validate_text.js <content file> --log <log file> [--review <review file>]`, exit 0 = valid ([[text-content-validator]]).
- data: `Content/RogueRigs/Data/Text/upgrade_texts.json`, `barks.json`, `alerts.json`; `Tools/content/logs/<run-id>.json` (log), `Tools/content/logs/<run-id>.review.json` (review).
- depends_on: [[sys-upgrades]], [[sys-hud]], [[sys-hud-pressure]], [[sys-objectives]].
- implements: [[assignment-4-plan]] content types 1–3, [[dialogue-tone]] T1 and T7.
- gotchas: the Text folder is staged as non-UFS through `RogueRigs/Data` (`Config/DefaultGame.ini`), read by the loader at runtime. Each run overwrites its content file; history is the log.

## Run id
- Pattern `^[0-9]{8}-[0-9]{6}-(upgrade_texts|barks|alerts)(-noretrieval)?$`: local date `YYYYMMDD`, time `HHMMSS`, the content file stem, and `-noretrieval` for the baseline run without lore (voice judgment). A `-noretrieval` content file lives under `Tools/content/logs/`, never under `Content/`.
- The stem must equal the content file name without `.json`.

## Envelope (all three content files)
- Pure JSON, UTF-8, no comments, no trailing commas. One object with exactly the keys `schema_version` (integer, `1`), `run_id` (string, pattern above), `items` (non-empty array of objects).
- An item holds exactly the keys of its file ([[text-content-files]]): a missing key, an extra key or a wrong type fails.
- Item ids are unique within a file. Texts are unique within a file (case-insensitive compare).

## Text rules (every text field)
- String, one line, no leading or trailing space, no double space, only printable ASCII (0x20 to 0x7E), so English only (T7).
- Words: tokens after splitting on spaces; punctuation stays attached ("down." is one word, "pre-flight" is one word).
- Word limits per field: [[text-content-files]].
- Numbers: an upgrade `flavor` contains no digit (values live in `UpgradeValues.csv` and the card `ValueText`, [[sys-upgrades]], so they cannot drift from [[upgrades]]). In a bark or alert text, every digit run must appear in an excerpt of that item's log entries (any loop) ([[text-run-log-schema]]).
- Vault examples, including those in these schema nodes, are reference only: a content text equal to one fails T8 (case-insensitive; the checked nodes are listed in [[text-content-validator]]).

## Relations
- [[pipelines-index]] — parent index.
- [[text-content-files]] — child: item fields, limits and an example per file.
- [[text-content-events]] — child: the allowed bark triggers and alert events and their C++ sources.
- [[text-run-log-schema]] — child: retrieval log and critic review files.
- [[text-content-validator]] — child: validator command, checks, error format, self test.
- [[agent-text]] — writes the files.
- [[agent-style-guide]] — reviews them.
