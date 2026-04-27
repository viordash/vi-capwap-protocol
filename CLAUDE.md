# Claude Code Instructions

## General

- Start each new conversation by reading `README.md` to understand the project context
- This is a C++17 library for CAPWAP protocol (RFC 5415/5416) packet definitions
- The library is designed to be included as a git submodule in other projects

## Naming Conventions

- Variables: `snake_case`
- Functions: `CamelCase`
- Do NOT use trailing underscore to mark member variables (e.g., use `secret` not `secret_`)
- File names: `PascalCase.h`, `PascalCase.cpp` (e.g., `ACName.h`, `WTPDescriptor.h`)

## C++ Style

- Use `struct` instead of `class`
- Use `__attribute__((packed))` for protocol structures
- Prefer `protected` over `private` (easier for testing)
- Do NOT implement methods in header files, even small ones — all implementations go in `.cpp` files
- Each element/struct must be in its own separate file (e.g., `ACName.h` + `ACName.cpp`)
- Use `#pragma once` for header guards

## Project Structure

```
src/
├── *.h, *.cpp          # CAPWAP message types (ConfigurationStatusRequest, etc.)
└── elements/           # CAPWAP message elements (ACDescriptor, WTPDescriptor, etc.)
tests/
├── unit_tests/         # Unit tests (CppUTest)
└── benchmarks/         # Performance benchmarks
docs/
├── rfc5415.txt         # CAPWAP Protocol Specification
└── rfc5416.txt         # CAPWAP Binding for IEEE 802.11
```

## Build

```bash
# Build all (tests + benchmarks)
make all

# Build only unit tests
make build_tests

# Build only benchmarks
make build_benchmarks

# Clean
make clean
```

## Tests

- Testing framework: CppUTest
- If a code module is in a separate file, its tests must be in a separate test file (e.g., `ACName.h` → `ACName_tests.cpp`)
- All tests in one test file must belong to a single TEST_GROUP
- Use this pattern for TEST_GROUP:
  ```cpp
  TEST_GROUP(XXXXTestsGroup){
      //
      TEST_SETUP(){}

      TEST_TEARDOWN(){}
  };
  ```
- Do NOT call `mock().checkExpectations()` manually — it is called automatically
- Use PublicMorozov pattern to access protected members in tests:
  ```cpp
  struct TestableClass : public OriginalClass {
      auto PublicMorozov_ProtectedMethod(args) {
          return ProtectedMethod(args);
      }
      auto &PublicMorozov_protected_field() {
          return protected_field;
      }
  };
  ```

## Code Quality

```bash
# Check code formatting
make check_format

# Apply code formatting
make apply_format
```

## Workflow

- Do NOT run `git commit` automatically — only suggest commit message text
- User will commit manually

## Commit Messages

- Keep commit messages short and concise
- Use conventional commits format: `type: short description`
- Types: `build`, `feat`, `fix`, `refactor`, `test`, `docs`, `chore`, `tooling`
- Do NOT add `Co-Authored-By` line
- Language: English

Example:
```
feat: add WTPRadioInformation element for IEEE 802.11 binding
```

## Pull Request Format

- Language: English
- Format: Markdown

### Structure

```markdown
## <Stage Name>: <Short Description>

<Introductory paragraph: context, what is implemented, key achievement>

### Main Changes

#### 1. <Change Category>

<Description of changes>

**Key files**: `path/to/file.cpp`, `path/to/file.h`

<Code examples where appropriate>

#### 2. <Next Category>

...

#### N. Tests

<Description of added tests>
```

### Guidelines

- Start with context: what was done in previous PR, what this PR adds
- Group changes by logical categories
- Mention specific files and classes affected
- Include code snippets for API changes
- Use bullet points for lists of changes
- Number main sections (1, 2, 3...)
- Explain WHY, not just WHAT
