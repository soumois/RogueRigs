---
id: text-content-validator
description: "validate_text.js usage: command, review auto-detection, what it checks, JSON error lines, the vault nodes it reads, self test. Open before running or extending it."
parent: text-content-schema
kind: doc
---
# Text content validator

Implements every rule of [[text-content-schema]], [[text-content-files]], [[text-content-events]] and [[text-run-log-schema]]. Node, no dependencies (task-074).

## Usage (repo root)
- `node Tools/content/validate_text.js <content file> --log <log file>`: content file plus its retrieval log.
- Add `--review <review file>` to also validate the critic review. Without it, `<log dir>/<run-id>.review.json` is used when it exists; a log with loop 1–2 entries needs a review.
- `node Tools/content/validate_text.js --selftest`: runs every case of `Tools/content/tests/`, exit 0 when all behave as expected.

## Result
- Exit 0: everything valid (`valid: <file>` on stderr). Exit 1: one JSON line per problem on stdout, `{"item_id", "check", "message"}` (`item_id` null for file-level problems). Exit 2: usage error.
- Fix every line, re-run, until exit 0.

## Checks
- Envelope, exact keys and types, run id pattern and stem, unique ids and texts, `-noretrieval` content only under `Tools/content/logs/`.
- Text rules, word limits (`T1-word-count` for barks), enums, id patterns, stat names, trigger and event ids, digits (`number-in-flavor`, `number-not-in-excerpt`), `T8-example-reuse`.
- Log: keys, `retrieval` vs run id, node ids in `vault/_paths.md`, `excerpt-verbatim` (CRLF → LF only), loop-0 entry per item, highest-loop `output` equals the content text.
- Review: rounds 0–2 in order, one review per item per round, pass/fail fields, rule ids, suggestion text rules; every correction answers a fail of the previous round; a fail without the next-loop correction (or a fail in round 2) is `failing-item`.

## Vault data read at run time
- Bark triggers and alert events: [[text-content-events]] tables. Stat names: the Display line of [[sys-upgrades]]. Rule ids: `- L<n>` of [[lore]] and `- T<n>` of [[dialogue-tone]].
- T8 corpus: every double-quoted string of 3+ words in [[dialogue-tone]], [[crew-characters]], [[company-voice]], [[lore]], [[fictional-films]] and the four schema nodes.
- Changing those line formats breaks the validator; run the self test after editing them.

## Tests
- `Tools/content/tests/<case>/`: content file, log, optional review, `expected.jsonl` (exact error lines; empty = must pass).
- `invalid-barks`: bad tone, 9-word bark, unknown trigger, excerpt absent from its node. `invalid-upgrade-texts`: digit in a flavor. `valid-barks`: one correction loop with its review.

## Relations
- [[text-content-schema]] — parent: the contract validated.
- [[agent-text]] — runs it before handing files over.
