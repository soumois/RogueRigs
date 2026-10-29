---
id: task-359-capture-tooling
description: "Global task: Capture tooling: crops and labelled comparison sheets with ffmpeg built in, and a generated list of rr.* debug console commands per system."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Capture tooling: crops, comparison sheets, debug command list

- request: owner, 2026-10-29, in French, verbatim: "Fait de 1 à 3 et est-ce que tu penses qu'un modéle sonnet suffirait pour les captures ? Peut être aussi prévoir un agent pour ça ? Par exemple, on lui donne une liste de chose à vérifier, mais il n'a pas besoin d'avoir tout le contexte. Je suppose qu'actuellement, l'agent BP ou CPP qui vérifie les captures et celui qui a créé ce qu'il doit vérifier". "1 à 3" = the three levers proposed by the main session the same day; this task is lever 2 (capture subtasks, 18 cpp runs 4.24 M, avg 22 turns, task-353-02 54 turns 0.76 M; target about -40 % of capture cost). Context relayed by the main session (cost analysis of 2026-10-29, `node vault/scripts/agent-usage.js --from 280` and subagent transcripts, weighted M units): tasks 332-357 cost ~49 M; workers 27.6 (bp 13.3, cpp capture subtasks 4.24 over 18 runs, 3d 3.6, art 2.4, doc 1.24), executor 6.3, planning ~9.7 (~20 %), main sessions ~5.4. Lever 4 (main session per request) is not requested. Lever 2 as proposed: each capture run rediscovers its tooling (lists Saved/Captures, greps `require` in Tools/capture, searches ffmpeg in `$LOCALAPPDATA/Microsoft/WinGet/Packages`, builds crops and comparison sheets with ad-hoc ffmpeg commands, greps the C++ source for `rr.*` commands such as GrantOre, shop, rarity). Proposal: `Tools/capture` gains crop and comparison-sheet options with the ffmpeg path built in, and the debug console commands are listed per system. Owner answer 2026-10-29 (relayed by the main session, verbatim): "OK, tout par défaut" = the plan and every default of Q1 to Q12 accepted (owner intent for the rule edits of `_navigation` and `orchestration-protocol` steps 3 and 4 given through Q1, Q2, Q7).
- goal: A capture run builds its crops and comparison sheets and finds the debug commands it needs with one documented command each, without searching the disk or the source.
- acceptance: `Tools/capture/sheet.js` builds a labelled comparison sheet and crops from given PNGs with no ffmpeg path argument; `Tools/capture/commands.js [filter]` prints every `rr.*` console command and variable with its help text and source file; tool doc node created and linked from pipelines-index and agent-cpp; lint clean; committed.
- gdd_check: crew tooling, no gdd node involved (Planner, 2026-10-29). Read: `Tools/capture/capture.js` (already finds ffmpeg on PATH or in WinGet packages, `findFfmpeg`, but only for its own contact sheet), agent-cpp (`capture_script`, `capture_rules`), sys-cheats (lists the rr.Cheat, rr.Debug, rr.Run, rr.Shop, rr.Menu commands in one dense paragraph; capture subtask task-355-02 had it as input and its run still grepped the source), task-355-02. Console registrations exist in 13 source files (Debug, Run, Menu, RunLog, Enemies, FlowField, Pickups), ~82 `rr.*` names. A hand-written command node would duplicate sys-cheats and sys-stress ([[_authoring]] forbids duplicated facts) and go stale, hence Q5. No tool doc node exists for capture.js today (only the agent-cpp field).
- confirmed_by_owner: yes (2026-10-29, "OK, tout par défaut" to Q1-Q12 and the plan, relayed by the main session)
- milestone: M2
- next_step: task-359-02-capture-tooling-doc
- started: 2026-10-29T14:17:36Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-29 (owner "OK, tout par défaut"): every default below is accepted; values provisional, owner 2026-10-29.
- Q5 Debug command list: generated on demand by `node Tools/capture/commands.js [filter]` from the C++ source (each registered `rr.*` command or variable: name, help text, file, grouped by source folder = system), instead of a hand-written vault node. Default: yes; the tool doc node points to it; sys-cheats unchanged.
- Q6 `node Tools/capture/sheet.js`: (a) `--out <png> --cols N <label>=<png> ...` labelled grid of images scaled to a common height (before / after / mockup); (b) `--crop x,y,w,h` in pixels or in fractions of the frame (so one crop works at every resolution) with `--zoom`, applied to every input; (c) ffmpeg found by the function capture.js already has, moved to a shared module. capture.js itself unchanged otherwise. Default: yes.

## Subtasks
1. task-359-01-capture-tooling-scripts (vault/reference/tasks/task-359-capture-tooling/task-359-01-capture-tooling-scripts.md) — cpp: Tools/capture/sheet.js (crops, labelled comparison sheet, shared ffmpeg finder) and commands.js; change summary.
2. task-359-02-capture-tooling-doc (vault/reference/tasks/task-359-capture-tooling/task-359-02-capture-tooling-doc.md) — doc: new tool doc node tool-capture (capture.js, sheet.js, commands.js) under pipelines-index; agent-cpp capture_script points to it.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-359-01-capture-tooling-scripts]] — step 1, cpp.
- [[task-359-02-capture-tooling-doc]] — step 2, doc.
- [[task-360-capture-agent]] — runs after this task; its definition uses these tools.
- [[task-358-planner-cost]] — lever 1, same request.
