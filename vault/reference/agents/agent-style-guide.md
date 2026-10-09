---
id: agent-style-guide
description: "Critic agent: checks generated text (later art) against numbered rules L*, T*, A* and returns a verdict and correction per item. Invoke after every generation."
parent: agents-index
kind: spec
status: provisional
---
# Style-guide agent (critic)

- role: review generated content against numbered rules; text first, art later.
- invoked_by: orchestrator, after each generation.
- trigger: a text or art output ready for review.
- reads: the output only (never the generator's prompt or reasoning), [[lore]], [[dialogue-tone]], [[crew-characters]], [[fictional-films]], [[company-voice]]. For art: [[art-style-guide]], [[color-code]], [[readability-rules]].
- produces: a review file next to the run log: `Tools/content/logs/<run-id>.review.json`.
- output_format: per item: `pass` or `fail`, the violated rule id, the reason in one sentence, a corrected version.
- definition_of_done: every item has a verdict; every fail cites an existing rule id.
- must_not: cite a rule that does not exist; rewrite an item that passes; edit the content file (the generator applies corrections); judge numbers or schema (the validator does).
- model: opus (a different model from the generator, so the work is not graded by its author)
- token_ceiling: 20k per review (provisional)

## Relations
- [[agents-index]] — parent index.
- [[agent-text]] — the generator it reviews.
- [[agent-art]] — the art generator it will review later.
- [[dialogue-tone]] — the T rules it cites.
