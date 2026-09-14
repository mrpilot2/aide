#!/usr/bin/env python3
"""Aggregate clang-tidy --enable-check-profile JSON output and print the
slowest checks summed across all analyzed translation units."""

import glob
import json
import sys
from collections import defaultdict


def main() -> int:
    if len(sys.argv) != 2:
        print(f"usage: {sys.argv[0]} <clang-tidy-profile-dir>", file=sys.stderr)
        return 1

    profile_dir = sys.argv[1]
    files = glob.glob(f"{profile_dir}/**/*.json", recursive=True)
    if not files:
        print(f"no profile JSON files found under {profile_dir}", file=sys.stderr)
        return 1

    totals = defaultdict(float)
    counts = defaultdict(int)
    for path in files:
        with open(path) as f:
            data = json.load(f)
        for key, value in data.get("profile", {}).items():
            if not key.endswith(".wall"):
                continue
            check = key[len("time.clang-tidy.") : -len(".wall")]
            totals[check] += value
            counts[check] += 1

    ranked = sorted(totals.items(), key=lambda kv: kv[1], reverse=True)
    print(f"{'check':<55} {'total wall (s)':>15} {'files':>7}")
    for check, total in ranked[:30]:
        print(f"{check:<55} {total:>15.3f} {counts[check]:>7}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
