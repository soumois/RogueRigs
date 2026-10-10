---
id: lesson-02-gdd-anatomy
description: "Lesson 02 (Sep 22): GDD as a short spec plus separate docs, scope, one agent one wow, pillars with veto, the three AI pillars."
parent: lessons-index
kind: note
---
# Lesson 02 — The GDD anatomy

## Key points
- The GDD is a spec, like spec-driven development: a contract that stops agents from drifting and producing fluff.
- 3–5 pages. Detailed docs live in separate files the GDD indexes (token cost, "lost in the middle"). The GDD is living and rewritten many times.
- Scope: a game buildable in 1–2 weeks for a 7-week course; a "2 devs, 48 h" test. A one-sentence pitch, or the agents will drift.
- One agent, one wow: an agent is a tool for one job, named by function, its file under about 2,000 words, loaded only when needed. Generic "game studio" crews lose to specialization.
- Pillars: 2–3 (4 max) with an automatic veto on any feature that serves none.
- The three AI pillars of a GDD: the dev pipeline (agents, how they talk, engine, frozen decisions; define inputs and outputs), technical constraints (API limits, cost ceiling), and the token budget (direct tokens usefully; cap per agent and task, watch for anomalies).
- AI delivers about 80 %; the human 20 % makes the fun.
- Asset specs as structured JSON (type, style, palette, dimensions, states) for fast iteration.
- Anti-goals are risky: writing "don't do X" can push the model toward X.
- The model is sycophantic: ignore "you're right", question everything, ask for 5 alternatives rather than one.

## Applies to Rogue Rigs
- The vault is the granular doc the short GDD indexes (owner's question in this lesson, confirmed by the instructor).

## Relations
- [[lessons-index]] — parent index.
- [[assignment-1-gdd-first-draft]] — the assignment set in this lesson.
