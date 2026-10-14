---
id: cloud-session-git
description: "Git from a cloud Claude session linked to the owner's PC: lock-file workaround, commit identity, trailers, selective staging, tags, push by the owner."
parent: agents-index
kind: note
---
# Git from a cloud session linked to the PC

The cloud container reaches the repo through the desktop-app bridge (shell on the PC, repo mounted at `$HOME/mnt/RogueRigs`). Deleting files is not permitted there by default, so git cannot remove its own files.

## After every commit
- Git leaves `.git/HEAD.lock`, `.git/index.lock`, `.git/objects/maintenance.lock` and `tmp_obj_*` files, and prints `unable to unlink`. The next git command would fail with "File exists".
- Move each lock to a new name directly under `.git/`, for example `mv .git/index.lock .git/stale-index-lock-N`. Never move a lock a running git may hold. The owner deletes `.git/stale-*` from time to time.
- `rm` works only after the permission tool for deletion is approved; the move needs no approval.

## Commit rules
- No git identity is configured in the cloud: pass `-c user.name=… -c user.email=…` taken from `git log -1 --format=%an` and `%ae`.
- End the message with the attribution lines of the session prompt (Co-Authored-By and Claude-Session).
- Stage explicit paths only. The Orchestrator may have uncommitted working-tree changes in the same checkout: never commit them. Check `git status --short` first.
- Run `node vault/scripts/lint.js --write` before committing vault changes; the pre-commit hook runs the lint.

## Tags and push
- A tag marks the last commit of an assignment (`a3-submission`, `a4-submission`). Hashes written in the READMEs afterwards come in a later commit, so the tag does not include them.
- Pushing is not possible from the cloud: the owner runs `git push origin master --tags`, then checks the GitHub rendering in a private window.

## Relations
- [[agents-index]] — parent index.
- [[orchestrator-runbook]] — the local git prefix and the commit message format of tasks.
- [[orchestrator-script-quirks]] — task-script defects, including the hard-coded attribution line.
