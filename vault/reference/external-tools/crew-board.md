---
id: crew-board
description: "Crew board: Claude mod showing vault tasks, subtasks, running agents, tokens and cost in a pane. Open to install, use or change it."
parent: external-tools-index
kind: doc
---
# Crew board

- responsibility: follow the dev crew in real time: tasks as cards (newest number first), their subtasks, the agents running now, tokens, units and launches.
- main_files: `Tools/crew-board/hooks/register.tsx` (the mod), `types/index.d.ts` (state contract), `usage_of.js` (transcript reader), `build_icons.js` and `hooks/icons.ts` (mascots), `hooks/crew-board.test.tsx`.
- entry_points: the pane opens at session start (option `openAtStart`); `/crew`, typing `/crew`, or the "Crew board" button above the prompt reopen it.
- data: see [[crew-board-data]]. Nothing is written: the mod only reads.
- depends_on: Claude Code with mods enabled, `node` on the PATH (for `usage_of.js`), the vault task nodes.
- implements: none (dev tooling, no gdd node).
- gotchas: native buttons keep the app's own colours and the text inside a button ignores colour props, so card text sits outside buttons; the whole-card click is transparent button rows laid over the card; icons are SVG and show on desktop surfaces only (terminal shows a coloured glyph).

## Install
Set in `~/.claude/settings.json`, `env`: `CLAUDE_CODE_PLUGIN_DIRS` = absolute path of `Tools/crew-board` and `CLAUDE_CODE_PLUGIN_DIR_WATCH` = `1`, then start a new session. Options (`plugin.json`): `refreshSeconds` (5), `tasksDir`, `transcriptsDir`, `openAtStart`. Defaults point at this repository and this machine.

## Screens
- List: filters All, To do, In progress, Wait, Done, Cancel (each with its count), pages, one card per task (status icon, title, bar, steps, tokens and units). A click anywhere on a card opens it.
- Task: title with status, description and the agents running, a foldable token table (workers, executor, planner, total), an Executor card, a Planner card, one card per subtask that unfolds its goal, tokens, run and result.

## Check
`claude plugin validate Tools/crew-board` and `claude plugin test Tools/crew-board` (five tests: fake vault, snapshot costs, live agents, helper script).

## Relations
- [[external-tools-index]] — parent index.
- [[crew-board-data]] — sources and rules behind every number.
- [[crew-board-assets]] — the mascots and the palette.
- [[orchestration-protocol]] — the task statuses the board shows are the ones its steps write.
