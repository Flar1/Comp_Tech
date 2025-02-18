#!/bin/sh

FILE="main.cpp"

REGEXP1="[0-9']*[\.][0-9']+[lf]?"
REGEXP2="[0-9']+[\.]"
REGEXP3="[0-9']*[\.][0-9']+[eE][+-]?[0-9']*[lf]?"
REGEXP4="0[xX][0-9a-fA-F']*\.[0-9a-fA-F']*[pP][0-9a-fA-F']+"

FLOAT_REGEXP="$REGEXP1|$REGEXP2|$REGEXP3|$REGEXP3"

grep -E -o "$FLOAT_REGEXP" "$FILE"