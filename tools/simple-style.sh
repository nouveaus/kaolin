#!/bin/bash

# red bold "error: " prefix
print_error() {
  echo -e "\033[1;31merror:\033[0m $1"
}

exit_code=0

whitespace_output=$(git grep -Pe '[ \t]$')
if [[ -n "$whitespace_output" ]]; then
  print_error "detected files with trailing whitespace"
  echo "$whitespace_output"
  exit_code=1
fi

cr_output=$(git grep -Il $'\r')
if [[ -n "$cr_output" ]]; then
  print_error "detected files with CR"
  echo "$cr_output"
  exit_code=1
fi

no_newline_files=$(git ls-files --cached --others --exclude-standard -z | xargs -0 -L1 bash -c 'test "$(tail -c 1 "$0")" && echo "- $0"')
if [[ -n "$no_newline_files" ]]; then
  print_error "detected files with no trailing newline:"
  echo "$no_newline_files"
  exit_code=1
fi

exit $exit_code
