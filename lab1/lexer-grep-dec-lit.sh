#!/bin/sh

FILE="main.cpp"

DEC_PATTERN="[1-9]([0-9']*[0-9])?"
SUFFIX_PATTERN="[uUlLzZ]*"


grep -E -o "\b(${DEC_PATTERN})${SUFFIX_PATTERN}\b" "$FILE"

