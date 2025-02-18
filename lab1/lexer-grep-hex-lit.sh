#!/bin/sh

FILE="main.cpp"

HEX_PATTERN="0[xX][0-9a-fA-F]([0-9a-fA-F']*[0-9a-fA-F])?"
SUFFIX_PATTERN="[uUlLzZ]*"


grep -E -o "\b(${HEX_PATTERN})${SUFFIX_PATTERN}\b" "$FILE"
