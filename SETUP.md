# PulseWaves Setup Guide

This guide helps you set up PulseWaves command-line tools and Python bindings on macOS.

## Quick Start

### 1. Build Command-Line Tools (Already Done!)

The command-line tools have been compiled for macOS:

```bash
# Tools are located in the root directory:
./pulseinfo -h
./pulse2pulse -h
```

**Available Tools:**
- **pulseinfo**: Display information about pulse waveform files
- **pulse2pulse**: Convert between pulse waveform file formats

### 2. Add Tools to Your PATH

To use the tools from anywhere:

```bash
# Add to your ~/.zshrc or ~/.bash_profile:
export PATH="$PATH:/Users/hmworsham/Repos/PulseWaves"

# Or create symlinks:
sudo ln -s /Users/hmworsham/Repos/PulseWaves/pulseinfo /usr/local/bin/pulseinfo
sudo ln -s /Users/hmworsham/Repos/PulseWaves/pulse2pulse /usr/local/bin/pulse2pulse
```

### 3. Set Up Python Bindings

#### Option A: High-Level Python Interface (Recommended)

This uses the compiled command-line tools via Python:

```bash
# Add the python directory to your PYTHONPATH
export PYTHONPATH="$PYTHONPATH:/Users/hmworsham/Repos/PulseWaves/python"

# Or install as a package
cd python
pip install -e .
```

Then use in Python:

```python
from pulsewaves import pulseinfo, pulse2pulse

# Get file information
info = pulseinfo("data.pls", verbose=True)
print(info)

# Convert formats
pulse2pulse("input.pls", "output.txt", verbose=True)
```

#### Option B: Native C++ Bindings (Advanced)

For direct access to the C++ library from Python:

```bash
# Install pybind11
pip install pybind11 numpy

# Build the native extension
cd python
python setup.py build_ext --inplace

# This creates pulsewaves_native module
```

Then use in Python:

```python
from pulsewaves.pulsewaves_native import PulseReader

reader = PulseReader()
if reader.open("data.pls"):
    header = reader.get_header()
    print(f"Number of pulses: {header['number_of_pulses']}")

    while reader.read_pulse():
        pulse = reader.get_pulse()
        print(f"Pulse at ({pulse['x']}, {pulse['y']}, {pulse['z']})")
```

## Usage Examples

### Command-Line Tools

```bash
# Display file information
./pulseinfo -i data/sofs_3d_2cycles.pls

# Display verbose information
./pulseinfo -i data/sofs_3d_2cycles.pls -verbose

# Convert PLS to text format
./pulse2pulse -i data/sofs_3d_2cycles.pls -o output.txt -verbose

# Convert between formats
./pulse2pulse -i input.lgw -o output.pls
```

### Python Interface

```python
# Example 1: Get file information
from pulsewaves import pulseinfo

info = pulseinfo("data/sofs_3d_2cycles.pls", verbose=True)
print(info['output'])

# Example 2: Convert formats
from pulsewaves import pulse2pulse

pulse2pulse(
    "data/sofs_3d_2cycles.pls",
    "output.txt",
    verbose=True
)

# Example 3: Read pulse data (if native bindings are built)
try:
    from pulsewaves.pulsewaves_native import PulseReader
    import numpy as np

    reader = PulseReader()
    if reader.open("data/sofs_3d_2cycles.pls"):
        header = reader.get_header()
        print(f"File has {header['number_of_pulses']} pulses")
        print(f"Bounds: X=[{header['min_x']}, {header['max_x']}]")

        # Read first 10 pulses
        for i in range(10):
            if not reader.read_pulse():
                break
            pulse = reader.get_pulse()
            waveform = reader.get_waveform()  # Returns numpy array
            print(f"Pulse {i}: position=({pulse['x']:.2f}, {pulse['y']:.2f}, {pulse['z']:.2f})")
            print(f"  Waveform samples: {len(waveform)}")

        reader.close()
except ImportError:
    print("Native bindings not available. Build with: cd python && python setup.py build_ext --inplace")
```

## Supported File Formats

### Input Formats
- `.pls` - PulseWaves format (uncompressed)
- `.plz` - PulseWaves format (compressed)
- `.lgw` - Leica Geosystems waveform data
- `.lgc` - Leica Geosystems waveform data (compressed)
- `.gcw` - GeoCoding Waveform
- `.sdf` - Sorted Data Format

### Output Formats
- `.pls` - PulseWaves format (uncompressed)
- `.txt` - ASCII text format

## File Locations

```
PulseWaves/
├── bin/                  # Source code for CLI tools
│   ├── pulseinfo.cpp
│   ├── pulse2pulse.cpp
│   └── Makefile
├── python/              # Python bindings
│   ├── __init__.py
│   ├── pulsewaves.py    # High-level interface
│   ├── pulsewaves_bind.cpp  # Native bindings source
│   └── setup.py
├── lib/                 # Compiled library
│   └── libpulsewaves.a
├── src/                 # Library source code
├── inc/                 # Header files
├── data/                # Sample data files
├── pulseinfo*           # Compiled executable
├── pulse2pulse*         # Compiled executable
└── SETUP.md            # This file
```

## Troubleshooting

### Command Not Found

If you get "command not found", make sure:
1. The tools are executable: `chmod +x pulseinfo pulse2pulse`
2. You're in the correct directory: `cd /Users/hmworsham/Repos/PulseWaves`
3. Or you've added them to your PATH

### Python Module Not Found

If Python can't find the module:

```bash
# Add to PYTHONPATH
export PYTHONPATH="/Users/hmworsham/Repos/PulseWaves/python:$PYTHONPATH"

# Or install it
cd python && pip install -e .
```

### Native Bindings Won't Build

Make sure you have:
```bash
pip install pybind11 numpy
```

If you still have issues, the high-level Python interface (Option A) works without native bindings.

## Next Steps

1. **Test with sample data**: Try the tools with files in the `data/` directory
2. **Integrate into your workflow**: Use the Python bindings in your scripts
3. **Build more tools**: Use the library to create custom pulse processing tools

For more information, see the original README files in the `PulseTools/` directory.
