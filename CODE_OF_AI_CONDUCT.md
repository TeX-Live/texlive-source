# Code of AI Conduct for TeX Live

(file version: $Id$)

This project accepts AI-assisted contributions, but human contributors
remain fully responsible, both technically and legally, for the changes
they submit.

Specifically, this implies that a knowledgable human must have reviewed
every line of code/documentation and tested the package's basic
functionality before uploading it.

## Requirements

- Follow the repository guidance in [`AGENTS.md`](AGENTS.md).

- Use AI as a tool, not as an authority. Verify behavior, correctness, and
  compatibility yourself before submitting changes.

- Any large code changes (~>100 lines) must be fully reviewed by
  at least two humans: the producer and a second reviewer.

- Keep changes narrow. Do not use AI to generate broad rewrites, drive-by
  refactors, or style-only churn.

- Do not submit generated code, text, or tests that you do not understand.

- Do not fabricate, or let the AI fabricate, benchmarks, bug reports,
  test results, reproduction steps, or anything else.

- Do not paste private code, non-public fonts or files, credentials, tokens,
  passwords, or any other confidential material into external AI systems.

- Respect licensing and attribution requirements. Do not submit AI-generated
  content that may have unclear provenance or incompatible licensing.

## Attribution

- When AI tools contributed meaningfully to a change, add an `Assisted-by:`
  trailer to the commit message (e.g.: `Assisted-by: Claude`).
  
- Routine use of autocompletion or spelling correction does not require
  attribution, but any nontrivial use of AI does. Most notably, if the
  majority or even a substantial minority of the code was written by AI,
  then disclose this.

- If a package appears to be AI-written (regardless of whether it
  actually is), we will apply our usual rules more strictly. Unlike
  other package ecosystems, all TeX package updates are processed by a
  team of human volunteers; we are happy to help humans if they missed
  something in the instructions, but we expect robots to be fully aware
  requirements from the beginning.

## Pull requests

- Always verify your changes against current sources before submitting.

- Report what you verified locally, including the exact build or test
  commands you ran. Should include a "make check" of the entire tree.

- Call out uncertainty, skipped validation, or areas that need extra review.

- Be prepared to revise or discard AI-generated changes that do not meet the
  project's standards.

## Maintainer expectations (that is, on the TeX Live side)

- Review AI-assisted contributions by the same technical standards as
  any other contribution.

- Prefer reproducible fixes, focused diffs, and adequate tests over volume.

- Reject changes that appear to be unreviewed AI output, including
  changes that add unnecessary complexity, invent or change behavior, or
  ignore repository guidance.

In short: if AI helped produce a change, a human contributor must still
own the result end to end.

(With thanks to Behdad Esfahbod for permission to use his version of
this file as a basis.)
