---
id: style-guide-text
description: "Scored text style guide: four constraint types (voice, lore, reference, format), rule mapping, 0-10 scale, threshold 8, F1-F2. Open before scoring or refining text."
parent: art-audio-index
kind: rule
applies_to: generated text, style-guide checks, style loop
---
# Style guide for generated text (scored)

The rules themselves live in [[dialogue-tone]] (T1–T9) and [[lore]] (L1, L2, L4); this node groups them into four constraint types and defines how a text is scored. Nothing here restates a rule.

## Constraint types and rules
| Dimension | What it judges | Rules |
|---|---|---|
| voice | Does the line sound like its speaker (Rook, Vesna, Kessler-Voss register)? | T2, T4, T5, T6 |
| lore | Does it fit the world and the objective? | L1, L2, L4, T9 |
| reference | Are film quotes, past jobs and example lines legitimate? | T3, T8 |
| format | Length, language, numbers. Measured by the validator, never by a model. | T1, T7, F1, F2, F3, F4 |

Format rules owned here:
- F1 Alert, upgrade and weapon lines: 3 to 12 words. Barks use T1.
- F2 Upgrade and weapon flavor contain no digit; values live in the data tables.
- F3 A number in any text must appear in a vault excerpt the writer retrieved; no invented form or notice numbers.
- F4 A film quoted as "Like in <Title>:" is a title of the [[fictional-films]] catalogue (checked by the loop script, not by a model).

## Scale
- Each item gets one integer 0 to 10 per model-judged dimension (voice, lore, reference). Format is 10 or 0 from the validator.
- 9 to 10: on-brand, nothing to change. 8: acceptable but flat. 5 to 7: partly off-brand, rewrite. 0 to 4: clear violation of a cited rule.
- An item passes when its lowest dimension is at least 8 (threshold 8).
- Every item gets a reason, passes included. A failing item cites the rule ids and a suggestion that names the exact change.

## Limits of the loop
- At most 2 refinement passes, then escalation to the owner ([[style-loop]]).
- Only the failing items are rewritten, and only their text field.

## Human-owned, not for the agent
The design and voice of Rook and Vesna, and any rule or catalogue change (new film title, new rule id), belong to the owner. The agent writes barks, alerts and flavor lines inside these rules. A rule it finds wrong is reported, never edited ([[agent-style-guide]]).

## Relations
- [[art-audio-index]] — parent index.
- [[dialogue-tone]] — T1–T9, the rules behind three of the four dimensions.
- [[lore]] — L1, L2, L4.
- [[agent-style-guide]] — the evaluator that applies this scale.
- [[style-loop]] — the loop that gates on this threshold.
