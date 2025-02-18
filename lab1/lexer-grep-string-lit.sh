#!/bin/sh

FILE="main.cpp"

STRING_LITERAL_PATTERN="(L|u8|u|U)?\"([^\\\"]|\\.)*\""

grep -E -o "$STRING_LITERAL_PATTERN" "$FILE"