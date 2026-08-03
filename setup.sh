#!/bin/bash
# Setup script for PulseWaves on macOS

set -e

echo "========================================="
echo "PulseWaves Setup for macOS"
echo "========================================="
echo ""

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

# 1. Check if tools are already built
echo "[1/4] Checking command-line tools..."
if [ -f "./bin/pulseinfo" ] && [ -f "./bin/pulse2pulse" ]; then
    echo "✓ Command-line tools already built"
else
    echo "Building command-line tools..."
    make clean
    make
    echo "✓ Tools built successfully"
fi

# 2. Make tools executable
echo ""
echo "[2/4] Making tools executable..."
chmod +x bin/pulseinfo bin/pulse2pulse 2>/dev/null || true
echo "✓ Tools are executable"

# 3. Python setup
echo ""
echo "[3/4] Setting up Python bindings..."

if command -v python3 &> /dev/null; then
    PYTHON_CMD=python3
elif command -v python &> /dev/null; then
    PYTHON_CMD=python
else
    echo "⚠ Python not found. Skipping Python setup."
    PYTHON_CMD=""
fi

if [ -n "$PYTHON_CMD" ]; then
    echo "Using: $PYTHON_CMD"

    # Check if numpy is installed
    if $PYTHON_CMD -c "import numpy" 2>/dev/null; then
        echo "✓ NumPy is installed"
    else
        echo "⚠ NumPy not found. Install with: pip install numpy"
    fi

    # Ask about native bindings
    echo ""
    read -p "Do you want to build native C++ bindings? (requires pybind11) [y/N]: " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        if $PYTHON_CMD -c "import pybind11" 2>/dev/null; then
            echo "Building native bindings..."
            cd python
            $PYTHON_CMD setup.py build_ext --inplace
            cd ..
            echo "✓ Native bindings built"
        else
            echo "⚠ pybind11 not found. Install with: pip install pybind11"
            echo "  Skipping native bindings (high-level interface will still work)"
        fi
    else
        echo "Skipping native bindings (you can build them later)"
    fi
fi

# 4. Setup instructions
echo ""
echo "[4/4] Setup complete!"
echo ""
echo "========================================="
echo "Next Steps:"
echo "========================================="
echo ""
echo "1. Test the command-line tools:"
echo "   bin/pulseinfo -h"
echo "   bin/pulse2pulse -h"
echo ""
echo "2. Add tools to your PATH (optional):"
echo "   echo 'export PATH=\"\$PATH:$SCRIPT_DIR/bin\"' >> ~/.zshrc"
echo "   source ~/.zshrc"
echo ""
echo "3. Use Python bindings:"
echo "   export PYTHONPATH=\"\$PYTHONPATH:$SCRIPT_DIR/python\""
echo ""
echo "   Or install as a package:"
echo "   cd python && pip install -e ."
echo ""
echo "4. Test with sample data:"
echo "   bin/pulseinfo -i data/sofs_3d_2cycles.pls"
echo ""
echo "For more information, see README.md"
echo ""
