---
id: task-362-agents-path-of
description: "Global task: agent definitions stop opening _paths.md; style-guide gets its fixed read paths in its definition, six agents resolve wikilinks with path-of.js."
parent: tasks-done-index
kind: task
status: done
---
# Agent definitions: path-of.js instead of _paths.md

- request: owner, 2026-10-29, in French, verbatim: "Oui, via le crew", answering the main session's question "Je fais passer style-guide sur path-of.js ?". Context relayed by the main session (read-count analysis of transcripts since 2026-10-27): the style-guide agent reads `vault/_paths.md` (~32 KB, ~9k tokens) early in every run (4 of 4 runs seen), so it is re-read on every following turn (~20k units per pass); the planner did the same before task-358, which added `vault/scripts/path-of.js` but did not update `.claude/agents/style-guide.md` nor agent-style-guide. The main session asked to check the other agent definitions and include them only if it is the same change (replace the `_paths.md` read by `path-of.js`), and noted that style-guide has tools Read and Write only (no Bash), so it cannot run path-of.js: how it gets its paths is asked to the owner (Q1). Owner answer 2026-10-29 (relayed by the main session, verbatim): "OK, tout par défaut" = the plan and the defaults of Q1, Q2 accepted.
- goal: No crew agent definition tells the agent to open `vault/_paths.md`: style-guide reads its fixed node list from resolved paths written in its definition, and the agents with Bash resolve wikilinks with path-of.js.
- acceptance: Per the decided Q1 and Q2: `.claude/agents/style-guide.md` step 1 lists the resolved paths of its reads (text set, image set, style-guide-text for scored mode), says the entity node path comes from the brief, never to open `vault/_paths.md`, and to stop and report a missing path; its tools stay Read, Write; agent-style-guide states the same. `.claude/agents/{3d,art,bp,cpp,doc,text}.md` no longer contain `_paths.md` and resolve wikilinks with `node vault/scripts/path-of.js <id> [<id>...]`; agent-3d, agent-art, agent-bp, agent-cpp, agent-doc, agent-text state it in one clause. `grep -l _paths .claude/agents/*.md` lists only planner.md (which says never to read it). Lint clean; committed. No agent run (none requested).
- gdd_check: crew tooling, no gdd node involved (Planner, 2026-10-29). Found: `.claude/agents/style-guide.md` never names `_paths.md`, but its step 1 sends it to the `reads` of agent-style-guide, which are wikilinks; with Read and Write only, `_paths.md` is its only way to resolve them, hence the read in every run. Its reads are a fixed set, resolved 2026-10-29 with path-of.js: lore (vault/reference/gdd/lore.md), dialogue-tone (vault/reference/gdd/art-audio/dialogue-tone.md), crew-characters (vault/reference/gdd/crew-characters.md), fictional-films (vault/reference/gdd/fictional-films.md), company-voice (vault/reference/gdd/company-voice.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), style-guide-text (vault/reference/gdd/art-audio/style-guide-text.md, already named by path in scored mode); the entity node comes from the caller (Executor step 4 hands resolved input paths). Same change elsewhere: `.claude/agents/3d.md`, `art.md`, `bp.md`, `cpp.md`, `doc.md`, `text.md` step 1 each say "Resolve a `[[wikilink]]` through `vault/_paths.md`"; all six have Bash, so path-of.js applies directly. Their vault nodes do not mention `_paths.md`. Not the same change, left out: executor.md (opens task nodes by printed path), capture.md, level-design.md (none names `_paths.md`; level-design lists its `reads` without a resolution rule, reported only), planner.md (done in task-358). Rule nodes `_navigation` (still allows `_paths.md` once per session) and `orchestration-protocol` are not edited.
- confirmed_by_owner: yes (2026-10-29, "OK, tout par défaut")
- milestone: M2
- next_step: none
- started: 2026-10-29T15:22:12Z
- waiting_for: none
- executor_tokens: input 16, cache write 35966, cache read 182576, output 2544
- executor_usage: runs 1, turns 7, context start 18k peak 36k, resumes 0, 3 min, units 0.08 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. Done, commit 98d28b42: style-guide has its nine read paths, six definitions and nodes use path-of.js. grep -l _paths still lists the seven files because the mandated sentence says 'never open vault/_paths.md'; none tells the agent to read it (intent met).

## Questions
Decided 2026-10-29 (owner "OK, tout par défaut"): every default below is accepted; values provisional, owner 2026-10-29.
- Q1 How style-guide gets its paths (tools Read, Write only). Options: (a) the resolved paths of its fixed reads are written in `.claude/agents/style-guide.md` step 1 (the nine paths in `gdd_check`), the entity node path comes from the brief as today, "never open `vault/_paths.md`; if a listed path is missing, stop and report it"; tools unchanged; risk: lint does not check `.claude/` files, so a moved node is caught only when the agent reports it; (b) add Bash to its tools so it runs path-of.js (a critic with a shell); (c) the caller resolves every path in each brief (Executor step 4 and the style loop change). Default: (a).
- Q2 Same change in the six other definitions that name `_paths.md`: in `.claude/agents/3d.md`, `art.md`, `bp.md`, `cpp.md`, `doc.md`, `text.md` step 1, "Resolve a `[[wikilink]]` through `vault/_paths.md`" becomes "Resolve a `[[wikilink]]` with `node vault/scripts/path-of.js <id> [<id>...]` (exact ids); never open `vault/_paths.md`", and agent-3d, agent-art, agent-bp, agent-cpp, agent-doc, agent-text get one clause saying so. Executor, capture and level-design are not touched. Default: yes, included in this task.

## Subtasks
1. task-362-01-agents-path-of-doc (vault/reference/tasks/task-362-agents-path-of/task-362-01-agents-path-of-doc.md) — doc: style-guide definition and node get fixed read paths; 3d, art, bp, cpp, doc, text definitions and nodes resolve wikilinks with path-of.js.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-362-01-agents-path-of-doc]] — step 1, doc.
- [[task-358-planner-cost]] — added path-of.js and moved the planner to it.
