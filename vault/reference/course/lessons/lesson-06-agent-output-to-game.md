---
id: lesson-06-agent-output-to-game
description: "Lesson 06 (Oct 6): integration checkpoint, engine formats, forcing structured output, translation layer, bad data, Generate → Validate → Review → Import, prototyping in pieces."
parent: lessons-index
kind: note
---
# Lesson 06 — From agent output to playable game

## Key points
- Integration checkpoint (pass/fail, before lesson 07): agent-generated content loads in the running game through the conversion step, with zero errors.
- Integration plan: which file and format, where it lands in the project, and the prompt "I have this file in this format, I use this engine, write a loader that creates these game objects". Think reusable systems: a format for every weapon, not one weapon.
- Formats: JSON (hierarchical), CSV (tables; Unreal DataTables need structs that match the columns exactly), plain text (UI strings, barks).
- Force structured output: "Respond ONLY with valid JSON. No explanations. No markdown. No preamble." Then a downstream validator, script or agent.
- Translation layer: agent schema → conversion script → validation → engine. Have the model write the script.
- Bad data: catch parse errors before the engine, diagnose instead of retrying, log every error with the raw output. Discard failed content, even outside the engine: if it stays in a doc, the model will read it and build on it.
- Generate → Validate → Review → Import. Start with human review at every step, relax only after a pipeline has proven itself.
- Prototype in pieces; mock-up images before UI code.
- Q&A: private repo → add the instructor as collaborator.

## Applies to Rogue Rigs
- The checkpoint is met by the layout pipeline: level-design JSON → `Tools/validate_layout.js` → C++ grid ([[agent-level-design]]).

## Relations
- [[lessons-index]] — parent index.
- [[assignment-4-content-pipeline]] — its loader follows this lesson.
