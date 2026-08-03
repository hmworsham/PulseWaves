# PulseWaves Python Bindings

Python interface for the PulseWaves full waveform LiDAR library.

## Installation

### Quick Install (High-Level Interface)

```bash
pip install -e .
```

This provides Python wrappers around the command-line tools.

### With Native Bindings (Recommended for Performance)

```bash
pip install pybind11 numpy
python setup.py build_ext --inplace
pip install -e .
```

This compiles C++ bindings for direct access to the PulseWaves library.

## Usage

### High-Level Interface

Works immediately after installation:

```python
from pulsewaves import pulseinfo, pulse2pulse

# Get file information
info = pulseinfo("data.pls", verbose=True)
print(info['output'])

# Convert formats
pulse2pulse("input.pls", "output.txt", verbose=True)
```

### Native Interface

If you built the native bindings:

```python
from pulsewaves.pulsewaves_native import PulseReader
import numpy as np

# Open and read pulse file
reader = PulseReader()
if reader.open("data.pls"):
    # Get header information
    header = reader.get_header()
    print(f"Number of pulses: {header['number_of_pulses']}")
    print(f"Bounds: X=[{header['min_x']:.2f}, {header['max_x']:.2f}]")

    # Read pulses
    while reader.read_pulse():
        pulse = reader.get_pulse()
        waveform = reader.get_waveform()  # numpy array

        print(f"Position: ({pulse['x']:.2f}, {pulse['y']:.2f}, {pulse['z']:.2f})")
        print(f"Intensity: {pulse['intensity']}")
        print(f"Waveform samples: {len(waveform)}")

    reader.close()
```

## API Reference

### High-Level Functions

#### `pulseinfo(filename, verbose=False)`

Get information about a pulse file.

**Parameters:**
- `filename` (str): Path to the pulse file
- `verbose` (bool): Include detailed information

**Returns:**
- dict: File information with 'output' and 'raw' keys

#### `pulse2pulse(input_file, output_file, **kwargs)`

Convert between pulse file formats.

**Parameters:**
- `input_file` (str): Input file path
- `output_file` (str): Output file path
- `**kwargs`: Additional arguments (e.g., `verbose=True`)

### Native Interface

#### `PulseReader`

##### Methods

- `open(filename: str) -> bool`: Open a pulse file
- `close()`: Close the file
- `read_pulse() -> bool`: Read the next pulse
- `get_pulse() -> dict`: Get current pulse data
- `get_header() -> dict`: Get file header
- `get_waveform() -> np.ndarray`: Get waveform samples
- `get_npoints() -> int`: Get total number of pulses

##### Pulse Dictionary Keys

- `x`, `y`, `z`: Spatial coordinates
- `intensity`: Pulse intensity value
- `offset`: Offset value
- `anchor_x`, `anchor_y`, `anchor_z`: Anchor point
- `target_x`, `target_y`, `target_z`: Target point
- `first_returning_sample`, `last_returning_sample`: Sample indices
- `descriptor_index`: Descriptor index

## Examples

See [SETUP.md](../SETUP.md) for more examples and usage patterns.

## Supported Formats

- `.pls` - PulseWaves uncompressed
- `.plz` - PulseWaves compressed
- `.lgw` - Leica waveform data
- `.lgc` - Leica compressed
- `.gcw` - GeoCoding waveform
- `.sdf` - Sorted data format

## Development

### Building from Source

```bash
# Install dependencies
pip install pybind11 numpy setuptools

# Build native extension
python setup.py build_ext --inplace

# Run tests (if available)
pytest tests/
```

### Project Structure

```
python/
├── __init__.py           # Package initialization
├── pulsewaves.py         # High-level Python interface
├── pulsewaves_bind.cpp   # Native C++ bindings
├── setup.py              # Installation script
└── README.md             # This file
```

## License

GNU Lesser General Public License v2 or later (LGPLv2+)

## Links

- [PulseWaves Website](http://pulsewaves.org)
- [GitHub](http://github.com/PulseWaves)
