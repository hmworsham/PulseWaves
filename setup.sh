#!/bin/bash

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

if command -v python3 > /dev/null 2>&1; then
    PYTHON_CMD=python3
elif command -v python > /dev/null 2>&1; then
    PYTHON_CMD=python
else
    echo "Python was not found on PATH."
    exit 1
fi

echo "Building libpulsewaves.a..."
make

echo "Installing PulseWaves Python bindings with $PYTHON_CMD..."
cd python
"$PYTHON_CMD" -m pip install -e .
