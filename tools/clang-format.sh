#!/bin/bash

if ! command -v "${CLANG_FORMAT}" &>/dev/null; then
  CLANG_FORMAT=clang-format-19
fi

if ! command -v "${CLANG_FORMAT}" &>/dev/null; then
  CLANG_FORMAT=clang-format
fi

find kernel/src/ \( -iname '*.h' -o -iname '*.c' \) -print0 | xargs -0 "${CLANG_FORMAT}" "$@" -i
