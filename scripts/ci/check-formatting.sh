#!/usr/bin/env bash

set -eo pipefail

# sudo apt-get update
# sudo apt-get install -y clang-format

python3 scripts/tools/format-code.py --check \
sc-kpm \
sc-memory \
sc-network \
sc-server \
tools