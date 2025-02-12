#!/bin/sh

FILE="main.cpp"

REGEXP='"([^"\\](\.[^"\\])*)"'

grep -E -o "$REGEXP" "$FILE"
