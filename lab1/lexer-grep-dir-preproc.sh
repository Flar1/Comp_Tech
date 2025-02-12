#!/bin/sh

FILE="main.cpp"

REGEXP="^\s*#\s*(define|include|if|ifdef|ifndef|elif|else|endif|pragma|error|warning|line)\b.*"

grep -E -o "$REGEXP" "$FILE"