---
id: assignment-4-content-pipeline
description: "Assignment 4, dynamic content pipeline: brief, deliverables, rubric, deadline, status. Open first for any Assignment 4 work, then its plan and open decisions."
parent: assignments-index
kind: note
---
# Assignment 4 — Dynamic content pipeline

## Brief
- Build a pipeline that generates content for the game using the GDD as source material. The agent reads the game docs before generating, so the output sounds like this game, not generic content.
- Deliverables: the pipeline (script, notebook or LLM-assisted workflow); three generated outputs the game actually needs; a short README: what content was generated, does it sound like the game, what did the critic agent catch.
- Placeholder lore gets no credit on content quality or game connection. The consistency check loop must be fully functional. Code that does not run scores 0 on every criterion.

## Rubric (10 points)
| Criterion | Points | Expected |
|---|---|---|
| Game-anchored source | 2.0 | the knowledge base is the GDD lore or its direct extension |
| Content fit | 2.5 | the three types are ones this game needs; the README names the gap and shows the output fills it |
| RAG implementation | 2.0 | output reflects retrieved context; query, retrieved chunk and output shown side by side |
| Consistency checking | 2.0 | the critic catches and corrects at least one lore break or tone drift; the correction is shown |
| Voice judgment | 1.5 | self-assessment, with at least one concrete prompt or retrieval tweak |

## Deadline
October 12, 2026. Time open: the lesson 05 slide says 23:59 BST (15:59 Pacific); the brief the owner pasted says 11:59 BST (03:59 Pacific). Plan for the earlier one until the owner confirms.

## Status
Done. Final commit `0790c65`, tag `a4-submission`, after task 085. Folder: `assignments/A4-dynamic-content-pipeline/`. Plan: [[assignment-4-plan]]. Decisions: [[assignment-4-open-decisions]].

## Relations
- [[assignments-index]] — parent index.
- [[assignment-4-plan]] — decisions taken and the pipeline design.
- [[assignment-4-open-decisions]] — what the owner must decide before generation.
- [[lesson-05-rag-content]] — the lesson that set it.
- [[lesson-06-agent-output-to-game]] — how its output reaches the game.
