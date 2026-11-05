@AGENTS.md

## Claude-specific
- The owner starts crew work with `/task <request>` (`.claude/commands/task.md`). Without it, the main session still follows the same workflow for any request that produces game content, code or assets: it only relays. It launches the `planner` agent (`agent-planner`, protocol steps 0 to 4), relays the owner's questions and answers (on a pure "ok" / "tout par défaut" it runs `node vault/scripts/task-confirm.js <Saved/Planner/<first-global-id>.defaults.json> "<owner words>"` instead of resuming the planner; any other answer, or a missing file, resumes it), then launches one fresh `executor` agent per confirmed global task (`agent-executor`, steps 5 and 6): one Opus agent that does the whole task itself and launches only `style-guide`. It never plans or executes itself.
- Sub-agent definitions live in `.claude/agents/`; each one points to its node in `vault/reference/agents/`.
