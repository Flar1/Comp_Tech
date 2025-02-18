#!/bin/sh

FILE="main.cpp"

OCT_PATTERN="0[0-7]([0-7']*[0-7])?"
SUFFIX_PATTERN="[uUlLzZ]*"

grep -E -o "\b(${OCT_PATTERN})${SUFFIX_PATTERN}\b" "$FILE"


