#!/usr/bin/env bash
# Runs clang-tidy, with the same options the dev preset uses at build time,
# on only the .cpp files changed against a base ref (default: HEAD, i.e. the
# uncommitted working tree), including untracked files. Much faster feedback
# than a full `cmake --workflow --preset workflow-dev-...` rebuild.
#
# Usage: tools/tidy-changed.sh [base-ref] [build-dir]
#   base-ref   git ref to diff against (default: HEAD; e.g. develop)
#   build-dir  configured build dir with compile_commands.json
#              (default: build/dev-unix-static-debug)
set -euo pipefail

repo_root=$(git rev-parse --show-toplevel)
cd "$repo_root"

base_ref=${1:-HEAD}
build_dir=${2:-build/dev-unix-static-debug}

if [[ ! -f "$build_dir/compile_commands.json" ]]; then
    echo "No $build_dir/compile_commands.json - configure and build the dev preset once first:" >&2
    echo "  cmake --preset dev-unix-static-debug && cmake --build --preset build-dev-unix-static-debug" >&2
    exit 2
fi

mapfile -t files < <(
    {
        git diff --name-only --diff-filter=d "$base_ref" -- '*.cpp'
        git ls-files --others --exclude-standard -- '*.cpp'
    } | sort -u
)

if [[ ${#files[@]} -eq 0 ]]; then
    echo "No changed .cpp files against $base_ref."
    exit 0
fi

echo "clang-tidy on ${#files[@]} file(s) changed against $base_ref (build dir: $build_dir)"

# clang-tidy reports every suppressed system-header warning count on stderr;
# drop those lines but keep xargs' exit status (non-zero on any finding).
set +e
printf '%s\0' "${files[@]}" | xargs -0 -P "$(nproc)" -n 1 \
    clang-tidy --quiet \
    -p="$build_dir" \
    -extra-arg=-Wno-unknown-warning-option \
    -extra-arg=-Wno-ignored-optimization-argument \
    -extra-arg=-Wno-unused-command-line-argument \
    -extra-arg=-std=c++20 \
    -warnings-as-errors='*' 2>&1 |
    grep -v -E '^[0-9]+ warnings? generated\.$'
status=${PIPESTATUS[1]}

if [[ $status -eq 0 ]]; then echo "clang-tidy: clean"; fi
exit "$status"
