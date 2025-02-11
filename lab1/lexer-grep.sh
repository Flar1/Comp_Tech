#!/bin/sh

DIR="folly"

REGEXP="^int "

for f in `find -name "main.cpp"`; do
    echo "*** File $f"
    grep -E $REGEXP $f
done
