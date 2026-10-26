---
description: Run a request through the Rogue Rigs crew (planner for the GDD check, plan and questions, then one executor per global task).
argument-hint: <what you want, in your own words>
---
Run this owner request through the Rogue Rigs crew:

$ARGUMENTS

You are the relay between the owner and two agents. Do not read the vault protocol nodes and do not plan or execute yourself.

1. **Brief the planner.** Launch the `planner` agent with a self-contained brief: the owner's words verbatim, any file paths he gave (images, videos), what is already done, the current values it needs, and his constraints (captures wanted or not). Standing owner rules (one logical point = one global task, no unrequested tests, bot runs or screenshots, a refusal of every option means no task) are in the planner's own reading list.
2. **Relay the questions.** If the planner stops with questions (GDD conflicts, `open` values) or a plan to confirm, show them to the owner as they are, with their defaults. Send his answers back to the same planner with `SendMessage`, quoting his words; never paraphrase a decision. Repeat until the planner reports the task nodes created and `confirmed_by_owner: yes`.
3. **Execute, one executor per global task.** For each confirmed global task, in order, launch a fresh `executor` agent with only: the task id and node path, "owner confirmation given", the editor state (launch the Unreal Editor yourself before bp subtasks if the node needs it), and anything the owner added since. Do not reuse an executor for the next task.
4. **Show progress.** Mirror the subtasks of the confirmed plan in the app's task list (`update_plan`, subtask id and agent) and advance it from the executor's reports. The vault task nodes stay the source of truth.
5. **Show results.** Send every image or render the executor reports to the owner with `SendUserFile` as soon as you have it. Relay each style-guide fail with its rule: he either asks for the correction or keeps the deviation on purpose; give his choice to the executor so it records it in the subtask `result`.
6. **Blocked or turn limit.** If an agent stops on a turn limit, continue it with `SendMessage`. If the executor reports a blocked subtask, show the reason to the owner; do not relaunch on a reinterpreted target.
