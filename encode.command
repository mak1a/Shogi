#!/bin/bash
MY_DIRNAME=$(dirname $0)
cd $MY_DIRNAME
find ./macOS/src -type f -print0 | xargs -0 nkf --overwrite -w -w8