---
id: task-081-01-a4-readme
description: "A4 README: assignments/A4-dynamic-content-pipeline/README.md filled from the real logs and reviews."
parent: task-081-a4-readme
kind: task
status: todo
---
# A4 README

- goal: A4 README: assignments/A4-dynamic-content-pipeline/README.md filled from the real logs and reviews.
- agent: claude
- depends_on: task-080-01-text-display
- inputs: assignment-4-content-pipeline (vault/reference/course/assignments/assignment-4-content-pipeline.md), assignment-4-plan (vault/reference/course/assignments/assignment-4-plan.md), lesson-05-rag-content (vault/reference/course/lessons/lesson-05-rag-content.md)
- expected_output: README with: the gap filled per content type, links to the 3 content files, a table of query / retrieved excerpt / output taken from the real logs, at least one critic correction with before and after, the voice self-assessment with the concrete tweak against the baseline run, how to run the pipeline. Nothing that was not produced. No assignment history in the root README.
- done_when: every claim traced to a log or a file; links resolve; lint clean.
- editor_state: any
- token_ceiling: 20000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-081-a4-readme]] — parent global task.
