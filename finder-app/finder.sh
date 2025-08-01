#!/bin/sh

if [ "$#" -ne 2 ]; then
  echo "Error: incorrect arugments"
  exit 1
fi

filesdir="$1"
if [ ! -d "$filesdir" ]; then
    echo "Error: dir not found"
    exit 1 
fi

searchstr=$2
numLines=0
numLines=$(grep -o -r "$searchstr" "$filesdir" | wc -l)

numFiles=0
numFiles=$(find "$filesdir" -type f | wc -l)

echo "The number of files are " $numFiles " and the number of matching lines are " $numLines 