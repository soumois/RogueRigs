---
id: task-402-01-map-path-repair
description: "Cpp: capture.js and Run-StressTest.ps1 repair a /Game map path rewritten by Git Bash (MSYS), one launch check"
parent: task-402-capture-map-path-msys
kind: task
status: done
---
# Game launches: /Game map path rewritten by Git Bash: cpp

- goal: Tools/capture/capture.js: before building the UnrealEditor arguments, turn any `--map` value of the form `<drive>:/<...>/Git/[usr/]Game/<rest>` back into `/Game/<rest>` (match on the `/Git/` or `/Git/usr/` segment followed by `Game/`, case-insensitive, not on the literal `C:/Program Files`), same repair on `/Game/` occurrences inside `--exec`, and print one line `map: repaired MSYS path -> /Game/<rest>` when it did; update its header usage comment. Scripts/Run-StressTest.ps1: same repair on `-Map` with the same printed line. No other behaviour change.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), sys-stress (vault/reference/tech/systems/tools/sys-stress.md), `Tools/capture/capture.js` (map at line 151, args at line 157), `Scripts/Run-StressTest.ps1` (`-Map` param, `$GameArgs`), Evidence: `Saved/Captures/T399-1920x1080/run.log` (rewritten, `LogInit: Command Line:`), `Saved/Captures/T399-all/run.log` (correct), task-402-capture-map-path-msys (vault/reference/tasks/task-402-capture-map-path-msys.md) (gdd_check findings, Q1 and Q2 answers)
- expected_output: Edited Tools/capture/capture.js and Scripts/Run-StressTest.ps1, one commit `task-402-01-map-path-repair: ...`; change summary with the commit hash, the repair rule and the check output.
- done_when: From Git Bash, `node Tools/capture/capture.js --name T402-msys --map /Game/RogueRigs/Maps/L_ArenaM1 --exec "rr.Debug.Delayed 4 rr.Debug.Shots 1,rr.Debug.Delayed 8 Quit" --expect 1` exits 0, prints `map: repaired MSYS path -> /Game/RogueRigs/Maps/L_ArenaM1`, and `Saved/Captures/T402-msys/run.log` has a `LogInit: Command Line:` line starting with `/Game/RogueRigs/Maps/L_ArenaM1` and no "could not be found" line (one launch only); the same command with `--map none` and without `--map` still behaves as before (no launch needed: no repair line printed for the default); Run-StressTest.ps1 repair checked on the string `C:/Program Files/Git/Game/RogueRigs/Maps/L_ArenaM1` without launching the game; committed by path.
- editor_state: any
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 24, cache write 38130, cache read 332760, output 9621
- usage: turns 12, context start 12k peak 38k, resumes 0, 2 min, units 0.13 M
- runs: 1
- milestone: M2
- started: 2026-10-30T23:50:36Z
- waiting_for: none
- result: Done, commit 7fe5d356: capture.js (--map and --exec) and Run-StressTest.ps1 (-Map) repair MSYS-rewritten /Game paths; T402-msys launch Command Line starts with /Game/RogueRigs/Maps/L_ArenaM1, no could-not-be-found; PS repair checked on string without launch.

## Relations
- [[task-402-capture-map-path-msys]] — parent global task.
