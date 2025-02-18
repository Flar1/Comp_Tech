#!/bin/sh

FILE="main.cpp"

HEX_PATTERN="0[xX][0-9a-fA-F]([0-9a-fA-F']*[0-9a-fA-F])?"
OCT_PATTERN="0[0-7]([0-7']*[0-7])?"
DEC_PATTERN="[1-9]([0-9']*[0-9])?"
SUFFIX_PATTERN="[uUlLzZ]*"

REGEXP="\\b((${HEX_PATTERN}|${OCT_PATTERN}|${DEC_PATTERN})${SUFFIX_PATTERN})\\b"

# Поиск литералов
grep -E -o "$REGEXP" "$FILE"
