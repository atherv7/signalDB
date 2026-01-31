#!/bin/bash

set -u

RED='\033[0;31m'
GREEN='\033[0;32m'
RESET='\033[0m'

DEFAULT_RUNS=5
runs="${1:-$DEFAULT_RUNS}"

failed=0

echo "Running tests $runs time(s)"

for ((i = 1; i <= runs; i++)); do
  echo -n "Run $i/$runs: "
  if make test >/dev/null 2>&1; then
    echo -e "${GREEN}PASSED${RESET}"
  else
    echo -e "${RED}FAILED${RESET}"
    failed=1
  fi
done

exit $failed
