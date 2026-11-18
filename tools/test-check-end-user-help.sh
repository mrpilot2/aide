#!/usr/bin/env bash
# Self-test for tools/check-end-user-help.sh: the check must accept a clean
# help directory and reject a page containing any forbidden pattern.
#
# Usage: tools/test-check-end-user-help.sh

set -u

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
check="${script_dir}/check-end-user-help.sh"

work_dir="$(mktemp -d)"
trap 'rm -rf "${work_dir}"' EXIT

failures=0

expect_exit() {
    local expected="$1" description="$2" dir="$3"
    # Exit codes: 0 = clean, 1 = forbidden pattern found, 2 = usage error.
    "${check}" "${dir}" >/dev/null 2>&1
    local actual=$?
    if [[ ${actual} -ne ${expected} ]]; then
        echo "FAIL: ${description} (exit ${actual}, expected ${expected})"
        failures=$((failures + 1))
    else
        echo "ok:   ${description}"
    fi
}

clean_page='# Keymap

Open the **Settings** action from the **File** menu and select **Keymap**.
Shortcuts are listed per action. A shortcut may combine Shift, Alt and
other modifier keys; the aside and FAQ do not trip the check.
'

make_help_dir() {
    local name="$1" extra="$2"
    local dir="${work_dir}/${name}"
    mkdir -p "${dir}"
    printf '%s' "${clean_page}" >"${dir}/keymap.md"
    if [[ -n "${extra}" ]]; then
        printf '%s\n%s\n' "${clean_page}" "${extra}" >"${dir}/bad.md"
    fi
    echo "${dir}"
}

expect_exit 0 "clean help directory" "$(make_help_dir clean '')"
expect_exit 0 "shipped help directory" "${script_dir}/../aide/help"

forbidden=(
    "mixed-case|Built with aIDE."
    "upper-case|Built with AIDE."
    "template|Welcome to {{ app_name }}."
    "ctrl|Press Ctrl+S to save."
    "alt|Press Alt+F4 to quit."
    "shift|Press Shift+Tab."
    "cmd|Press Cmd+, on macOS."
    "f1|Press F1 for help."
    "f12|Press F12 for tools."
)

for entry in "${forbidden[@]}"; do
    name="${entry%%|*}"
    line="${entry#*|}"
    expect_exit 1 "rejects '${line}'" "$(make_help_dir "${name}" "${line}")"
done

expect_exit 2 "missing directory" "${work_dir}/does-not-exist"

if ((failures > 0)); then
    echo "${failures} check(s) failed"
    exit 1
fi
echo "all checks passed"
