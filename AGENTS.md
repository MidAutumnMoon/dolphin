# AGENTS.md

This is a personal fork of KDE Dolphin file manager, tuned to my own habits.

## Rules

- This is a C++20 / Qt6 / KDE Frameworks 6 project. Use KDE conventions: `slotFooBar()` slots, `Q_EMIT`, `i18n()` for user-visible strings. Prefer `this->member` over `m_` prefix for new code — match existing style in old files.
- `kitemviews/` is a custom view framework — not QListView. Rendering bugs are easy to introduce here. Tread carefully.
- Settings live in `.kcfg` schemas → `.kcfgc` generates C++ classes. Adding a new setting means touching both plus migration.
- New dependencies go in `flake.nix`. This is a Nix-managed build.
- Use `clang-format` for style. The repo has a `kde_clang_format` CMake target.
- Prefer `.cc`/`.hh` extensions for new C++ files, not `.cpp`/`.h`.
- Existing files follow KDE convention: `DolphinRemoveAction` → `dolphinremoveaction.cpp`. For new files, use snake_case: `dolphin_remove_action.cc`. No tooling enforces this — just rename the file and update CMakeLists.
- Prefer functions over methods. If it doesn't need `this`, make it a free function in a namespace (anonymous namespace if only one call site). Qt forces OOP at the framework boundary — let it — but go functional in the middle.
- When making a user-visible change, add an entry to the **Fork Changes** section in `README.md`. Describe what changed, not how — no implementation details.

## What changes look like here

- **Change a default** → find the .kcfg entry, change `default=`
- **Add a shortcut** → find the action, add the shortcut in the .rc file
- **Tweak a behavior** → find where it happens, guard or adjust it
- **Change how items render** → `kitemviews/` — be careful

## Look Things Up

- When unsure about a library, tool, or API, use web search or Context7 before guessing.
- Prefer Context7 for library docs — it pulls real examples and up-to-date signatures.
- Don't hallucinate option names, function signatures, or CLI flags. Look it up.

## Complex Tasks

- Break large tasks into sub-tasks. Tackle them in parallel with sub-agents when they don't depend on each other.
- Give each sub-agent full context — it won't see your conversation history.
- Keep sub-tasks scoped to one concern. If two sub-agents might edit the same file, don't run them in parallel.

## Communication

- Be short. Say the thing, stop.
- Don't repeat what I already said or what's already in context.
- Don't pad with disclaimers, summaries, or "hope that helps" type closings.
- If something is wrong, say what's wrong and how to fix it. Don't hedge.
