---
id: lesson-03-gdd-to-prototype
description: "Lesson 03 (Sep 24): evaluating GDD foundations, stress zones, synthetic personas and review crews, limits of AI feedback, revising for specificity, wishlist."
parent: lessons-index
kind: note
---
# Lesson 03 — From GDD to prototype

## Key points
- Check three foundations early: a coherent core loop, agent roles without overlap and without a master agent, realistic token budgets.
- Stress zones: scope creep (no playable prototype after 3 weeks means too big), vague roles (describe an agent by what it produces), missing mechanics ("what happens when the player does X?").
- Synthetic audience: personas (optimizer, explorer, casual) read the same GDD in isolation and score it; a review crew (exploit hunter, narrative consistency, pacing and flow) finds logic gaps. Watch for sycophancy.
- Agents find logic gaps; humans find fun gaps. Ask humans: what does the player do? Is it fun? What is missing? Reread after 24 hours.
- Revising: a revised GDD is more specific, not longer. Name each agent and its output format (example: dialogue lines in JSON with `speaker_id`, a `tone` from a fixed list of 5, a word limit, checked by a style guide agent). Describe exact actions, cut what one person cannot build, keep a separate wishlist.
- Q&A: in Unreal, have the AI write C++ rather than Blueprints; 3D assets through Meshy, Blender MCP, Unreal MCP, expect cleanup; build the first 5 minutes or one boss rather than an incomplete whole.

## Applies to Rogue Rigs
- The final GDD took this format for the text agent; [[agent-text]] and [[agent-style-guide]] inherit it.

## Relations
- [[lessons-index]] — parent index.
- [[assignment-2-gdd-final-draft]] — the assignment set in this lesson.
