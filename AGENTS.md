# AGENTS.md

(file version: $Id$)

TeX Live is the main source for the TeX typesetting system. Both
backward and forward compatibility are top priorities. Keep changes
narrow, preserve behavior unless the task explicitly requires otherwise,
and avoid style-only churn.

TeX Live includes many third-party software packages which are
maintained elsewhere. Reports about those packages should be sent to the
relevant upstream location.

## Read first

Before non-trivial work, read:

- `README`
- `README.md`
- `README.*`

## Core rules

- Prefer minimal diffs. Do not rename, reorder, or reformat unrelated code.

- Scope changes to one subsystem or concern whenever possible, limiting
changes to be as small as possible. Prefer incremental follow-up commits
over large rewrites.

- Public API and ABI of the included libraries, such as Kpathsea, are
hard constraints. The TeX language itself and the TeX program's behavior
must not change in any way whatsoever unless explicitly asked. Do not
add new API unless explicitly asked. Do not change or remove existing
API/ABI unless explicitly asked.

- Follow local conventions in the touched file.

- Preserve optional-feature behavior, reduced-feature builds, and
compile-time feature guards.

- TeX Live uses Automake for its build system. No other build system is
supported. Patches must relate any configuration changes to the existing
build system. New files, whether sources, tests, or anything else,
should be mentioned in the relevant Makefile.am file (at least).

- Keep out-of-memory behavior in mind. New code should fail safely and
follow existing allocation and error-handling patterns.

- Do not touch unrelated user changes and untracked artifacts.

## Repo map

- `am/`: common fragments for Makefile.am files.
- `build-aux/`: common utility scripts, from GNU gnulib and related.
- `doc/`: the tlbuild document; read it for background.
- `libs/`: third-party libraries; don't install new ones.
- `m4/`: Autoconf macros used in configure.ac.
- `texk/`: programs that depend on the `kpathsea` library.
- `utils/`: programs that do not depend on `kpathsea`.

Third party sources not maintained in TeX Live are imported into
directories ending with `-src`, such as `libs/harfbuzz/harfbuzz-src`.
Changes in such directories should be reported upstream, according to
the upstream's preferred conventions and requirements.
(These third-party -src directories are sometimes slightly patched for
the TeX Live build; such patches are in a sibling `TLpatches`
subdirectory.)

## Build and verify

Default flow:

```sh
./Build
```

This uses a subdirectory `./Work` to do the build, and installs the
resulting binaries and other files in a subdirectory `./inst`.

General expectations:

- Rebuild touched targets and make sure there are no new warnings.

- For changes to any program or library, rebuild the relevant tool and
exercise it on a small sample.

- When fixing a bug, add or update the nearest test and its expected output.

- Treat portability regressions as bugs. TeX Live must build on a
wide variety of systems (GNU/Linux, macOS, Windows, and many more
including old verson) and compilers (GCC, Clang, MSVC). System
variations such as 32/64-bit, BigEndian/LittleEndian and reduced-feature
builds must all remain supported.

## Avoid

- Do not make broad formatting passes.
- Do not change configure or make defaults or behavior unless required.
- Do not assume generated outputs, local fonts, or untracked artifacts
are disposable.
- Do not disable tests without having a valid reason to do so, and
documenting that reason.

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
submitted without running tests if they do not affect code, build logic,
generated outputs, or test inputs.

- Keep each commit focused on a single issue or concern. Do not combine
unrelated fixes, API renames, refactors, or behavior changes into one
commit unless explicitly asked.

- Never push to any remote repository. The user handles pushing.

- Write descriptive commit messages.

- Wrap commit message bodies to about 70 columns.

- For multi-line commit messages, write the message from a file or
editor-backed input. Do not pass escaped `\n` sequences via shell `-m`
arguments.

- Explain root cause, fix, and testing in the commit body when testing
was performed or is relevant.

- When relevant, link issues or PRs with trailers such as `Fixes:`.

- Always include an `Assisted-by:` or `Co-Authored-By` trailer on
commits written through the agent.

## Summary

- State assumptions briefly before large or risky changes.
- For multi-step work, give a short plan, then execute it.
- Keep explanations concise and tied to the modified files.

- Make minimal diffs focused on the requested scope.
- Preserve existing project patterns before introducing new abstractions.
- Run targeted tests for touched areas when feasible.

- Never add new third-party dependencies, on libraries, programs, or any
  other software.

- Make sure that all changes are fully compatible with (at least)
  Windows, macOS, and GNU/Linux, including older operating system
  releases. Similarly, all changes must be fully compatible with GCC,
  CLANG, MSVC, and other major compilers.

- All new changes must be accompanied by new tests, following the
  pattern of existing tests for the software component being changed.
  Besides the tests themselves, patches must also include changes
  to install the new tests into the Automake test infrastructure that is
  used for all of TeX Live.

(With thanks to Behdad Esfahbod for permission to use his version of
this file as a basis.)
