---
id: task-020-04-bp-check
description: "Open the editor, check every Blueprint compiles and loads without broken references, run a PIE smoke test of the full loop."
parent: task-020-code-review
kind: task
status: done
---
# Blueprint check after refactor

- goal: the refactor broke nothing in the editor.
- agent: bp
- depends_on: task-020-03-refactor-cleanup
- inputs: [[unreal-mcp]], [[sys-cheats]], [[blueprints-index]], task-020-02 and task-020-03 results.
- expected_output: compile every Blueprint under `/Game/RogueRigs` and report errors or warnings; output log scanned for redirector or missing-class errors at editor start; PIE smoke test with the cheat property route (level-up offer, console, shuttle) and no log error; fixes only if trivial (re-save, re-link), otherwise report precisely.
- done_when: all Blueprints compile; PIE smoke test passes with no error; lint clean.
- editor_state: open
- token_ceiling: 90000
- model: sonnet
- tokens: input 58, cache write 75678, cache read 1578396, output 8654
- runs: 1
- milestone: M1
- result: Editor start log clean for /Game/RogueRigs (no missing class, property or redirector); 19/19 Blueprints compiled with warnings as errors, 0 issue; L_ArenaM1 map check 0/0; PIE via cheat properties: invulnerable, GrantXP → level 2 offer (game paused) → pick, console drop, hack complete, shuttle with 5 s override → run lost; no PIE log error or warning. Win path not run. Nothing re-saved, no commit. Editor showed a 'source content files changed, import?' prompt (likely the runtime-read CSVs): left unanswered.

## Relations
- [[task-020-code-review]] — parent global task.
