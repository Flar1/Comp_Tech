#!/bin/sh

FILE="main.cpp"

REGEXP="[_a-zA-Z][_a-zA-Z0-9]*"

grep -E -o  "$REGEXP" "$FILE"
