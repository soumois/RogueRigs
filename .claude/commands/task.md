---
description: Run a request through the Rogue Rigs crew (planner for the GDD check, plan and questions, then one executor per global task).
argument-hint: <what you want, in your own words>
---
Run this owner request through the Rogue Rigs crew:

$ARGUMENTS

You are the relay between the owner and two agents. Do not read the vault protocol nodes and do not plan or execute yourself.

1. **Brief the planner.** Launch the `planner` agent with a self-contained brief: the owner's words verbatim, any file paths he gave (images, videos), what is already done, the current values it needs, and his constraints (captures wanted or not). Standing owner rules (one logical point = one global task, no unrequested tests, bot runs or screenshots, a refusal of every option means no task) are in the planner's own reading list.
2. **Relay the questions.** If the planner stops with questions (GDD conflicts, `open` values) or a plan to confirm, show them to the owner as they are, with their defaults. If his answer is a pure "ok" / "tout par défaut" and the planner reported a defaults file (`Saved/Planner/<first-global-id>.defaults.json`, printed by `task-create.js`), do not resume the planner: run `node vault/scripts/task-confirm.js <file> "<his words verbatim>"` (it marks the questions decided, sets `confirmed_by_owner` and `todo`, applies the edits, lints, commits); if it refuses or the file is missing, resume the planner. Any other answer goes back to the same planner with `SendMessage`, quoting his words; never paraphrase a decision. Repeat until the planner reports the task nodes created and `confirmed_by_owner: yes`.
3. **Execute, one executor per global task.** For each confirmed global task, in order, launch a fresh `executor` agent with only: the task id and node path, "owner confirmation given", the current editor state (open or closed; the executor opens and closes the editor itself as its subtasks need it), and anything the owner added since. Its prompt names the global task id before any other task number and its description does not start with a subtask id (cost tracking). Do not reuse an executor for the next task.
4. **Show progress.** Mirror the subtasks of the confirmed plan in the app's task list (`update_plan`, subtask id and role) and advance it from the executor's reports. The vault task nodes stay the source of truth.
5. **Show results.** Send every image or render the executor reports to the owner with `SendUserFile` as soon as you have it. Relay each style-guide fail with its rule: he either asks for the correction or keeps the deviation on purpose; give his choice to the executor so it records it in the subtask `result`.
6. **Blocked or turn limit.** If an agent stops on a turn limit, continue it with `SendMessage`. If the executor reports a blocked subtask, show the reason to the owner; do not relaunch on a reinterpreted target.
