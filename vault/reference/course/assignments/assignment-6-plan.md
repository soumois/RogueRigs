---
id: assignment-6-plan
description: "Assignment 6 plan written 2026-10-20: reuse map, owner needs, crew tasks, schedule to Oct 26, definition of done, open decisions (all proposals)."
parent: assignment-6-style-guide-agent
kind: note
---
# Assignment 6 — Plan (proposal; nothing decided yet)

## What already exists
- Rules: [[lore]] (L), [[dialogue-tone]] (T), [[company-voice]], [[crew-characters]], [[fictional-films]], [[art-style-guide]] (A), [[color-code]]. Constraint types available: tone, vocabulary and lore, length and format, reference conventions, color and shape.
- Generator: [[agent-text]]. Evaluator: [[agent-style-guide]], which returns pass or fail, a rule id, a reason and a corrected line: no score yet.
- Deterministic first: `Tools/content/validate_text.js`.
- Real content and verdicts: `Tools/content/logs/` (runs and `.review.json`; `baseline/` holds runs made without lore).
- Loop driver pattern: a crew command, like `/architect` ([[assignment-5-agent-design]]).

## Gaps to close
1. A consolidated style guide with at least three typed constraint groups, each traced to its source node.
2. Evaluator output: score per constraint type plus reason.
3. A refiner and a loop that run without a human, with a pass limit.
4. A run on real content, with three before and after items of distinct violation classes.

## Owner needs
Check the portal for the brief and numbering; answer the open decisions (20 min); answer the preparatory exercise; read the three examples and confirm they are real; validate the guide; push, tag, submit.

## Crew tasks
Style guide node; evaluator with scores; refiner and loop runner; run and logs; README; vault update; lint.

## Schedule (Pacific)
Tue Oct 20 decisions and portal check · Wed 21 and Thu 22 build · Fri 23 run and review · Sat 24 README and captures · Sun 25 buffer, tag, push · Mon 26 submit before 16:59.

## Definition of done
The loop runs from one command; every item has a score and a reason; fails are corrected by the refiner within the limit; three examples are in the README with their logs; the README is complete on its own.

## Open decisions (proposals)
1. Content: text, since the evaluator exists; images only as an extra.
2. Score 0 to 10 per constraint type; pass at 8 or more (7 saves tokens, 9 costs iterations: [[lesson-08-ger-loop]]).
3. At most 3 passes, then stop and report the root cause.
4. Before items from `baseline/` runs; after items from the automated loop.
5. Evaluator model different from the generator's.
6. Pipeline connection: after [[agent-text]] and before the validator's import step.
7. Preparatory exercise: safe, generic barks, loot icons, rock variants; unsafe, the main characters' designs. The owner confirms.

## Relations
- [[assignment-6-style-guide-agent]] — parent: brief and rubric.
- [[assignment-5-plan]] — the previous plan, same structure.
- [[lesson-08-constraints-and-demo]] — constraints as evaluator rules.
