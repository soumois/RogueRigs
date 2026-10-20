---
id: lesson-08-ger-loop
description: "Lesson 08 (Oct 13), part 1: Generate, Evaluate, Refine, deterministic checks first, scores, targeted refining, 3-pass circuit breaker, where the human sits."
parent: lessons-index
kind: note
---
# Lesson 08 — The GER loop

## Why
Outputs of the lesson 06 pipeline were good, broken, or technically valid but wrong for the game; reading each by hand does not scale. The goal is the quality of a full manual review with less reading time. The trickiest case is "valid but not fun or badly optimised".

## Generate, Evaluate, Refine
- Generate (a first draft, fast and varied, broken output expected), Evaluate (a second agent or rule set), Refine (back to generate on fail). Same idea as test-driven development.
- Order: deterministic scripts first (parsing, compile, tests, linters), agents only if they pass. A verifier agent is worth its cost for one-off content. Do not claim code catches everything: an agent can write a test that always passes.
- Several evaluators can each score one aspect from 0 to 10; a failing score goes straight back to refining.
- Four axes: GER on real development, a generator-evaluator contract (clear responsibilities), deterministic checks plus agent verifiers, a refinement strategy.

## Targeted refining
- The refiner gets the exact error and the original output and fixes that, no blind regeneration, no unrelated changes. Keep what works and inject it as a rule for the next generation.
- Up to 3 passes: pass 1 the smallest fix, pass 2 tighten with the same error context, pass 3 last try, then escalate with a clear statement of the problem. 5 or 8 once the loop is proven. A hard limit is mandatory.
- On escalation find the root cause: agent, criterion too strict, or requirements vague.
- Never trust "almost there, 90 %". Clear the context after a series of failures.

## The human
Score threshold: 9 means many more iterations and tokens, 7 may be enough if the owner finishes by judgment. Minimise tokens by refining single pieces; minimise time by refining more before a human. The human reviews the agents' report and data (performance graphs), and a human decides at the end whether a feature belongs in the game.

## Relations
- [[lessons-index]] — parent index.
- [[lesson-08-constraints-and-demo]] — part 2: constraints, demo, No Man's Sky.
- [[assignment-6-style-guide-agent]] — builds the evaluator loop.
