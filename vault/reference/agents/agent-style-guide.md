---
id: agent-style-guide
description: "Critic agent: checks generated text (L, T rules) and images (A rules), returns verdict and correction per item; a fail is an overrulable reminder."
parent: agents-index
kind: spec
status: provisional
---
# Style-guide agent (critic)

- role: review generated content against numbered rules: player-facing text, and candidate images. A fail is a reminder, not a veto: the owner may keep a deviation on purpose (owner 2026-10-14); the critic still reports it so a deviation is always a choice, never an oversight.
- invoked_by: orchestrator, after each generation.
- trigger: a text output or a set of candidate images ready for review.
- reads: the output only (never the generator's prompt, reasoning or self-review). Text: [[lore]], [[dialogue-tone]], [[crew-characters]], [[fictional-films]], [[company-voice]]. Images: [[art-style-guide]], [[color-code]], [[readability-rules]], and the entity node the Orchestrator names.
- definition_file: `.claude/agents/style-guide.md`.
- produces: text: `Tools/content/logs/<run-id>.review.json`; images: `Art/Source/<asset>/<run-id>.review.json`, next to the art run log.
- output_format: per item (text line or image file): `pass` or `fail`, the violated rule id, the reason in one sentence, a correction (a corrected line; for an image, the one change to ask in the next prompt).
- definition_of_done: every item has a verdict; every fail cites an existing rule id.
- must_not: cite a rule that does not exist; rewrite an item that passes; edit the content file or an image; judge numbers or schema (the validator does); drop a fail because the owner overruled the same rule before (it reports, he decides).
- model: opus (a different model from the generators, so the work is not graded by its author)
- token_ceiling: 20k per review (provisional)

## Relations
- [[agents-index]] — parent index.
- [[agent-text]] — the text generator it reviews.
- [[agent-art]] — the image generator it reviews.
- [[dialogue-tone]] — the T rules it cites.
- [[art-style-guide]] — the A rules it cites.
