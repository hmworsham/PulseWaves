# PulseWaves

This repository is a fork of PulseWaves/PulseWaves version 0.3 rev 11 (Martin Isenburg, rapidlasso GmbH). This fork enables the use of PulseWaves for waveform LiDAR processing on Unix-like systems by providing CLI and Python 3 wrappers for core PulseWaves functions, including `pulseinfo` and `pulse2pulse`. Wrappers for additional tools (`pulseview`, `pulsesort`, `pulseextract`, etc.) are in development. These tools parse and perform analytical operations on data stored in various waveform LiDAR formats (e.g. LVIS, LAS 1.3 FWF, GeoLas waveform, and PulseWaves).

## Table of Contents

- [Quick Start](#quick-start)
- [Installation](#installation)
- [Command-Line Usage](#command-line-usage)
- [Python Usage](#python-usage)
- [Supported File Formats](#supported-file-formats)
- [Project Structure](#project-structure)
- [Building from Source](#building-from-source)
- [Troubleshooting](#troubleshooting)

## Quick Start

### Build Everything

```bash
# Build library and command-line tools
make all

# Or build just the tools
make tools

# Optionally build Python bindings
make python
```

### Use the Command-Line Tools

```bash
# Display file information
bin/pulseinfo -i data/sofs_3d_2cycles.pls

# Convert between formats
bin/pulse2pulse -i data/sofs_3d_2cycles.pls -o output.txt
```

### Add Tools to Your PATH

To use the tools from anywhere:

```bash
# Add to your ~/.zshrc or ~/.bash_profile:
export PATH="$PATH:/path/to/PulseWaves/bin"

# Or create symlinks: 
sudo ln -s /path/to/PulseWaves/bin/pulseinfo /usr/local/bin/pulseinfo
sudo ln -s /path/to/PulseWaves/bin/pulse2pulse /usr/local/bin/pulse2pulse
```
Then, 

```bash
# Display file information
pulseinfo -i data/sofs_3d_2cycles.pls

# Convert between formats
pulse2pulse -i data/sofs_3d_2cycles.pls -o /path/to/output.gcw
```
## Installation

### Command-Line Tools (macOS)

The command-line tools are pre-compiled for macOS and located in the `bin/` directory:

```bash
# Available tools:
bin/pulseinfo -h
bin/pulse2pulse -h
```

**Available Tools:**
- **pulseinfo**: Display information about pulse waveform files
- **pulse2pulse**: Convert between pulse waveform file formats

### Python Bindings

This PulseWaves fork provides straightforward Python bindings to the compiled command-line tools via Python subprocess wrappers:

```bash
# Add the python directory to your PYTHONPATH
export PYTHONPATH="$PYTHONPATH:/path/to/PulseWaves/python"

# Or install as a package
cd python
pip install -e .
```

Then use in Python:

```python
from pulsewaves import pulseinfo, pulse2pulse

# Get file information
info = pulseinfo("data.pls", verbose=True)
print(info['output'])

# Convert formats
pulse2pulse("input.pls", "output.txt", verbose=True)
```

## Command-Line Usage

### pulseinfo

Display information about pulse waveform files:

```bash
# Basic information
bin/pulseinfo -i data/sofs_3d_2cycles.pls

# Verbose output
bin/pulseinfo -i data/sofs_3d_2cycles.pls -verbose
```

### pulse2pulse

Convert between waveform file formats:

```bash
# Convert PLS to text format
bin/pulse2pulse -i data/sofs_3d_2cycles.pls -o output.txt -verbose

# Convert between binary formats
bin/pulse2pulse -i input.lgw -o output.pls
```

## Python Usage

```python
from pulsewaves import pulseinfo, pulse2pulse

# Example 1: Get file information
info = pulseinfo("data/sofs_3d_2cycles.pls", verbose=True)
print(info['output'])

# Example 2: Convert formats
pulse2pulse(
    "data/sofs_3d_2cycles.pls",
    "/path/to.output.gcw",
    verbose=True
)
```

See [python/README.md](python/README.md) for complete API reference.

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

## Project Structure

```
PulseWaves/
├── README.md              # This file
├── COPYING.txt            # License (LGPLv2+)
├── Makefile               # Build orchestrator
│
├── bin/                   # Command-line executables
│   ├── pulseinfo          # Compiled binary
│   ├── pulse2pulse        # Compiled binary
│   ├── pulseinfo.cpp      # Source code
│   ├── pulse2pulse.cpp    # Source code
│   └── Makefile
│
├── python/                # Python bindings
│   ├── README.md          # Python API reference
│   ├── __init__.py
│   ├── pulsewaves.py      # High-level interface
│   ├── pulsewaves_bind.cpp # Native bindings source
│   └── setup.py
│
├── src/                   # Library source code
│   ├── Makefile
│   └── *.cpp
│
├── inc/                   # Header files
│   └── *.hpp
│
├── lib/                   # Compiled library
│   └── libpulsewaves.a
│
├── data/                  # Sample data files
├── examples/              # Usage examples
└── img/                   # Documentation images
```

## Building from Source

### Prerequisites

- C++ compiler (clang++ or g++)
- Make
- Python 3.x (for Python bindings)

### Build Commands

```bash
# Build library and tools
make all

# Build only the library
cd src && make

# Build only the tools
make tools

# Build Python bindings
make python

# Or manually:
cd python && python setup.py build_ext --inplace

# Install Python package
make install-python
# Or manually:
cd python && pip install -e .

# Clean build artifacts
make clean

# Deep clean (before git commit)
make clobber
```

## Troubleshooting

### Command Not Found

If you get "command not found":

1. Make sure the tools are executable: `chmod +x bin/pulseinfo bin/pulse2pulse`
2. Either run from the bin directory: `cd bin && ./pulseinfo -h`
3. Or add to your PATH (see [Quick Start](#quick-start))

### Python Module Not Found

If Python can't find the module:

```bash
# Add to PYTHONPATH
export PYTHONPATH="/path/to/PulseWaves/python:$PYTHONPATH"

# Or install it
cd python && pip install -e .
```

### Build Errors

```bash
# Clean and rebuild
make clobber
make all
```

## License

GNU Lesser General Public License v2 or later (LGPLv2+)

See [COPYING.txt](COPYING.txt) for details.

## Links

- [PulseWaves Website](http://pulsewaves.org)
- [Original GitHub](http://github.com/PulseWaves)

## Development

For developers working on PulseWaves:

1. **Making changes**: Edit source files in `src/`, `bin/`, or `python/`
2. **Testing**: Use sample data in `data/` directory
3. **Cleaning**: Use `make clobber` before committing to remove all build artifacts
4. **Contributing**: Follow the original PulseWaves coding conventions

## Examples

See the `examples/` directory for more usage patterns and sample scripts.
