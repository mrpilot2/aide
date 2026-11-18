#!/usr/bin/env bash
# Checks End-User Help pages against the authoring rules that a grep can
# enforce (see the End-User Help section of docs/contributing.md):
#   - never mention aIDE;
#   - no template variables ({{ ... }});
#   - no concrete key sequences (Ctrl+, Alt+, Shift+, Cmd+, F<n>).
#
# Usage: tools/check-end-user-help.sh [help-dir]   (default: aide/help)
# Exit codes: 0 = clean, 1 = forbidden pattern found, 2 = usage error.

set -u

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
help_dir="${1:-${script_dir}/../aide/help}"

if [[ ! -d "${help_dir}" ]]; then
    echo "error: help directory '${help_dir}' does not exist" >&2
    exit 2
fi

# POSIX ERE only (no \b), so the check behaves the same with GNU and BSD grep.
not_word='(^|[^[:alnum:]_])'
not_word_end='([^[:alnum:]_]|$)'

rules=(
    "mentions aIDE|${not_word}aide${not_word_end}"
    "template variable|\{\{"
    "key sequence|(ctrl|alt|shift|cmd)[[:space:]]*\+"
    "function key|${not_word}f[0-9]{1,2}${not_word_end}"
)

status=0
for rule in "${rules[@]}"; do
    description="${rule%%|*}"
    pattern="${rule#*|}"
    if matches="$(grep -R -n -i -E --include='*.md' -e "${pattern}" "${help_dir}")"; then
        while IFS= read -r match; do
            echo "${match}: ${description}"
        done <<<"${matches}"
        status=1
    fi
done

if ((status != 0)); then
    echo "End-User Help violates the authoring rules in docs/contributing.md" >&2
fi
exit "${status}"
