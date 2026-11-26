# Contributing

## Branching

`main` should represent the latest stable, publishable project state.

Use feature branches for in-progress work that is not ready to be released or
treated as stable. Merge to `main` when the work is coherent, tested enough for
its stage, and accurately documented.

## Commits

This project uses Conventional Commits.

Format commit subjects as:

```text
type: short imperative summary
```

Use these common types:

- `feat:` for new firmware, hardware, or project capabilities
- `fix:` for bug fixes or hardware design corrections
- `docs:` for documentation-only changes
- `chore:` for repo maintenance and config cleanup
- `refactor:` for restructuring without intended behavior/design changes
- `test:` for test-only changes

Use one commit body with a single bullet list when the change needs context:

```text
feat: add mouse pointer board source

- Import the cleaned PMW3610 board baseline.
- Preserve upstream license and attribution.
- Leave RJ45 design changes for a later commit.
```

## Releases

Releases mark usable hardware and firmware checkpoints.

- Use `v0.X.0` for a meaningful new device capability or a new hardware and
  firmware combination builders need to match.
- Use `v0.X.Y` for targeted hardware or firmware fixes to that milestone.
- Tag each release at the commit with the validated hardware source and matching
  firmware. Link to matching fabrication files from the hardware page.
- Publish website and guide updates as regular commits. If a guide correction
  affects an existing build, record it in the notes for that release.
- Do not move existing release tags.
- Do not put partial next-milestone work into a patch release.
- Update `CHANGELOG.md` when preparing a release, summarizing the commits since
  the previous release.
