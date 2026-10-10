---
id: lesson-05-rag-content
description: "Lesson 05 (Oct 1): RAG as reading the game docs first, content inventory, engine formats, tone control, generate 10 keep 3, critic agent."
parent: lessons-index
kind: note
---
# Lesson 05 — Dynamic content generation: RAG pipelines

## Key points
- RAG = the agent reads the game's own documents before writing anything. Without it, output comes from training data and sounds generic.
- At this scale the lore fits in the context window: no database needed. Separate `.md` files per topic; before generating, load the relevant sections (a merchant line → merchant lore, tone guide, world rules).
- A shared lore base is the single source of truth for every agent: fewer invented facts, consistency as the world grows. Decide what agents may generate alone and what needs a human gate.
- Content inventory: list the content types the game needs, how each is produced, what AI does badly, and the dependencies each type adds (dialogue needs a dialogue UI). Cut content that does not improve the player's experience.
- Structure for the engine: JSON for hierarchical data, CSV for tables. Plain text is not usable.
- Tone control: define the tone, each character's voice and each area's mood; without it agents invent one. Human gates early.
- Generate 10, keep 3: generate variants, keep the best, analyse why, feed that back into agents and docs.
- Critic agent: load the game docs, give the new content and the docs in one prompt, ask whether it contradicts the lore; yes → regenerate. The critic runs before the human gate.

## Applies to Rogue Rigs
- The vault is the lore base; [[assignment-4-content-pipeline]] applies this lesson.

## Relations
- [[lessons-index]] — parent index.
- [[assignment-4-content-pipeline]] — the assignment set in this lesson.
