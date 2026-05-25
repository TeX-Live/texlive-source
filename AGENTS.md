# AGENTS.md

(file version: $Id$)

TeX Live accepts AI-assisted contributions, but *human contributors
remain fully responsible, both technically and legally, for the changes
they submit*.

Specifically, this implies that a knowledgable human must review every
line of code/documentation and tested the package's basic functionality
before uploading or reporting.

TeX Live is the main source for the TeX typesetting system. Both
backward and forward compatibility are top priorities. Keep changes
narrow, preserve behavior, avoid style-only churn, and generally
minimize changes and descriptions.

TeX Live includes many third-party software packages which are
maintained elsewhere. Reports about those packages should be sent to the
relevant upstream location.

## Read first

- The high-level AI policy public web page at
  <https://tug.org/texlive/aipolicy.html>.

- Before non-trivial work, read the `README`, `README.md`, and `README.*`
  files at the top level of the source tree.

- Use AI as a tool, not as an authority. You (a human) should verify
  behavior, correctness, and compatibility yourself before submitting changes.
  Do not submit anything you don't understand.

- Any large code changes (>100 lines or so) must be fully reviewed by
  at least two humans before submission: the producer and a second reviewer.

- Do not fabricate, or let the AI fabricate (or hallucinate),
  benchmarks, bug reports, test results, reproduction steps, or anything
  else.

- Respect licensing and attribution requirements. Do not submit AI-generated
  content that may have unclear provenance or incompatible licensing.

## Core rules

- Prefer minimal diffs. Do not rename, reorder, or reformat unrelated code.

- Scope changes to one subsystem or concern whenever possible, limiting
changes to be as small as possible. Prefer incremental follow-up commits
over large rewrites.

- The public API and ABI of the included libraries, such as Kpathsea,
must not change. The TeX language itself and the TeX program's behavior
must not change. Do not add new API unless explicitly asked. Do not
change or remove existing API/ABI unless explicitly asked.

- Keep out-of-memory behavior in mind. New code should fail safely and
follow existing allocation and error-handling patterns.

- Do not touch unrelated user changes and untracked artifacts.

- Follow local conventions in the touched file.

- Preserve optional-feature behavior, reduced-feature builds, and
compile-time feature guards.

- TeX Live uses GNU Automake (basic files: `configure.ac`,
`Makefile.am`, `*.ac`) for its build system. No other build system is
supported. Patches must relate any configuration changes to the existing
Automake-based build system. New files, whether sources, tests, or
anything else, should be mentioned in the relevant `Makefile.am` or
other files.

## Repo map

- `am/`: common fragments for Makefile.am files.
- `build-aux/`: common utility scripts, from GNU gnulib and related;
  do not change them.
- `doc/`: the tlbuild document; read it for background.
- `libs/`: third-party libraries; do not require new ones.
- `m4/`: Autoconf macros used in configure.ac.
- `texk/`: programs that depend on the `kpathsea` library.
- `utils/`: programs that do not depend on `kpathsea`.

Third party sources not maintained in TeX Live are imported into
directories ending with `-src`, such as `libs/harfbuzz/harfbuzz-src`.
Changes in such directories should be reported upstream, according to
the upstream's preferred conventions and requirements. These
third-party `-src` directories are sometimes slightly patched for the
TeX Live build; such patches are in a `TLpatches/` subdirectory sibling
to the `-src`.

## Build and verify

Default flow:

```sh
./Build
```

This uses a subdirectory `./Work` to do the build, and installs the
resulting binaries and other files in a subdirectory `./inst`.

General expectations:

- Always verify changes against current sources before submitting.

- Rebuild touched targets and make sure there are no new warnings.

- For changes to any program or library, rebuild the relevant tool and
  exercise it on a small sample.

- When fixing a bug, add or update the nearest test and its expected output.

- All new changes must be accompanied by new tests, following the
  pattern of existing tests for the software component being changed.
  Besides the tests themselves, patches must also include changes to
  install the new tests into the Automake test infrastructure that is
  used for all of TeX Live.

- Treat portability regressions as bugs. TeX Live must build on a
  wide variety of systems (GNU/Linux, macOS, Windows, and many more,
  including old versions thereof) and compilers (GCC, Clang, MSVC).
  System variations such as 32/64-bit, BigEndian/LittleEndian and
  reduced-feature builds must all remain supported.

## Avoid

- Do not make broad formatting passes.

- Do not change `configure` or `make` defaults or behavior unless required.

- Do not assume generated outputs, local fonts, or untracked artifacts
  are disposable.

- Do not disable tests without having a valid reason to do so, and
  documenting that reason.

- Avoid extraneous material in general. Strive to minimize changes.

## Working mindset

- Reproduce the issue before fixing it, if at all possible.

- Understand root cause before writing code. If there is real ambiguity,
  ask instead of guessing.

- Validate simplifications carefully. If code looks unusually complex,
  it probably handles a real edge case. Check history carefully before
  making any simplifications. 

## Commit guidance

- Before any commit, run the entire test suite with `./Build`.

- Exception: simple documentation-only or CI-only changes may be
  submitted without running tests if they do not affect code, build
  logic, generated outputs, or test inputs.

- Keep each commit focused on a single issue or concern. Do not combine
  unrelated fixes, API renames, refactors, or behavior changes into one
  commit.

- Write descriptive commit messages.

- Wrap commit message bodies to about 70 columns.

- For multi-line commit messages, write the message from a file or
  editor-backed input. Do not pass escaped `\n` sequences via shell `-m`
  arguments.

- Explain root cause, fix, and testing in the commit body when testing
  was performed or is relevant.

- When relevant, link issues or PRs with trailers such as `Fixes:`.

- Do not paste private code, non-public fonts or files, credentials,
  tokens, passwords, or any other confidential material into external AI
  systems. Do not include them in submissions, either.

- Report what you verified locally, including the exact build or test
  commands you ran.

- Call out uncertainty, skipped validation, or areas that need extra review.

- Be prepared to revise or discard AI-generated changes that do not meet the
  project's standards.

## Attribution

- When AI tools contributed meaningfully to a change, add an `Assisted-by:`
  trailer to the commit message (e.g.: `Assisted-by: Claude`).
  
- Routine use of autocompletion or spelling and grammar correction does
  not require attribution.

- If a package appears to be AI-written (regardless of whether it
  actually is), we will apply our usual rules more strictly. Unlike
  other package ecosystems, all TeX package updates are processed by a
  very small team of human volunteers; we are happy to help humans if
  they missed something in the instructions, but we expect robots to be
  fully aware requirements from the beginning.

## Summary

- State assumptions briefly before large or risky changes.

- For multi-step work, give a short plan, then execute it.

- Keep explanations concise and tied to the modified files.

- Make minimal diffs focused on the requested scope.

- Preserve existing project patterns before introducing new abstractions.

- Run targeted tests for touched areas when feasible.  Create new tests
  for new functionality.

- Never add new dependencies on libraries, programs, operating system
  features, or anything else.

- Make sure that all changes are fully compatible with (at least)
  Windows, macOS, and GNU/Linux, including older operating system
  releases. Furthermore, all changes must be fully compatible with GCC,
  CLANG, MSVC, and other major compilers.

## How your submission will be reviewed by TeX Live maintainers

- We review AI-assisted contributions by the same technical standards as
  any other contribution.

- We do not rely on AI analysis to decide whether to include, reject, or
  request changes to a patch. We may use AI to help make a decision, but
  it is not used as a replacement for our judgement.

- We prefer reproducible fixes, focused diffs, and adequate tests over volume.

- We summarily reject changes that appear to be unreviewed AI output,
  including changes that add unnecessary complexity, invent or change
  behavior, or ignore repository guidance.

In short: if AI helped produce a change, a human contributor must still
own the result end to end.

## Scope of this policy = TeX Live

This policy is for TeX Live (<https://tug.org/texlive>). Other parts of
the TeX world, notably CTAN (<https://ctan.org>), as well as individual
package and program maintainers, have their own policies. Their policies
must also be complied with; TeX Live's policy does not override anyone
else's.

Public questions and comments about this policy should be sent to the
general mailing list, tex-live (at) tug.org
(<https://lists.tug.org/tex-live>). Private communication can be sent to
the security mailing list, tlsecurity (at) tug.org.

(With thanks to Behdad Esfahbod for permission to use his AI policies
for HarfBuzz as a basis for this.)
