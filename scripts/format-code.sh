#!/usr/bin/env bash

set -eo pipefail

python3 scripts/tools/format-code.py \
sc-kpm \
sc-memory \
sc-network \
sc-server \
tools