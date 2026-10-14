# Code Architect — blackboard

Every decision of the Code Architect (`/architect`, [source](../../.claude/commands/architect.md)) is written here **before** it acts on it, and committed section by section (`architect run N: <section>`). Past sections are never rewritten. The current state is in [state.md](state.md).

Sections of a run:
1. **Base:** commit the run starts from.
2. **Scan:** every GDD feature, `built` / `partial` / `missing`, with evidence.
3. **Scoring:** missing features ranked, score and reason per factor; excluded features and why.
4. **Recommendation:** the top buildable feature, then the owner's answer, quoted verbatim.
5. **Plan:** global task and subtasks.
6. **Build:** per subtask, the exact prompt sent to the subagent, then files, build and test result, commit.
7. **Rescan:** the feature is now `built`; final agent commit.
8. **Owner review:** what the owner changed, in a separate commit.

<!-- Runs are appended below by the agent. -->
