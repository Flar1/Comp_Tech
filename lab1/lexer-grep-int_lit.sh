#!/bin/sh

FILE="main.cpp"


REGEXP="\b(0[bB][01']+|[0-9']+|0[0-7']+|0[xX][0-9a-fA-F']+)([uU]?[lL]{0,2}|[lL]{0,2}[uU]?)\b"

grep -E -o  "$REGEXP" "$FILE"
