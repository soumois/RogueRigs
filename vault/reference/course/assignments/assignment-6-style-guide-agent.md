---
id: assignment-6-style-guide-agent
description: "Assignment 6, Style Guide Agent: brief, three components, rubric, deadline Oct 26, numbering caveat, status. Open first for any Assignment 6 work."
parent: assignments-index
kind: note
---
# Assignment 6 — Style Guide Agent

## Brief (lesson 09 slides)
Build the evaluator loop. From the capstone game, build a Style Guide Agent that encodes the game's own aesthetic rules and enforces them on content generated for it. The guide must derive from the GDD, the established art and tone references or earlier assignments, never from an invented universe. Three components:
1. A style guide grounded in the capstone.
2. An evaluator that returns a SCORE and a REASON per output, not pass/fail. The refiner uses the reason.
3. An automated iteration loop, shown end to end on real generated content: the agent finds violations and corrects them alone. It must prove it corrects, not only detects.

Outputs must cite concrete game elements (characters, factions, art style, setting, tone, voice). Generic or invented content scores zero on Game specificity and Before/after.

## Rubric (10 points)
| Criterion | Points | Expected |
|---|---|---|
| Game specificity | 3.0 | rules derive from the game; a stranger can tell which game; generic = 0 |
| Enforcement accuracy | 3.0 | violations found correctly; rewrites or flags make content more on-brand; judged against the owner's own guide |
| Before/after demonstration | 2.0 | three real examples, each a distinct violation class (tone, vocabulary, formatting…); placeholder = 0 |
| Style guide depth | 1.5 | at least three distinct constraint types, all game-specific |
| Pipeline connection | 0.5 | ONE sentence naming where it runs in the current workflow; a hypothetical future pipeline gets nothing |

## Preparatory exercise (slide "When to use which?")
List three assets of the game where AI generation is safe, and one where it would break the experience, with the reason. It is the base of the style guide.

## Deadline and numbering caveat
October 26, 2026, 23:59 GMT = 16:59 Pacific. The lesson 09 slides number this assignment 6 and call it mandatory. The course program lists "Build a GER pipeline" as 6 and the style guide as 7; lesson 08 slides gave no assignment. Owner 2026-10-20: Assignment 7 does not exist yet on the portal, so the lesson 09 numbering stands (style guide = 6). Whether a separate GER deliverable exists stays unverified. The lesson 09 summary has no transcript, only slides.

## Status
In progress (2026-10-20). Folder: `assignments/A6-style-guide-agent/` (README skeleton).

## Relations
- [[assignments-index]] — parent index.
- [[assignment-6-plan]] — who does what, reuse, schedule, open decisions.
- [[lesson-09-human-touch-style-guide]] — the lesson that sets it.
- [[lesson-08-ger-loop]] — generate, evaluate, refine, circuit breaker.
- [[agent-style-guide]] — the existing critic it extends.
