#!/bin/sh

if [ "$#" -ne 2 ]; then
  echo "Error: incorrect arugments"
  exit 1
fi

writefile="$1"

writestr="$2"
mkdir -p "$(dirname "$writefile")"

touch $writefile
echo $writestr > $writefile