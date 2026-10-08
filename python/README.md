# PulseWaves Python Bindings

This package exposes the PulseWaves C++ library through a native `pybind11` extension named `pulsewaves.pulsewaves_native`. Importing `pulsewaves` re-exports the wrapped classes, constants, and helper functions from that extension. The binding links against the static library built from `../src.

## Install for Development

From the repository root:

```bash
make
cd python
python -m pip install -e .
```

To rebuild the extension in place without reinstalling the package:

```bash
make python
```

When you use an in-place build, add the package directory to `PYTHONPATH`:

```bash
export PYTHONPATH="$PWD/python:$PYTHONPATH"
```

## Import the Bindings

```python
import pulsewaves as pw

print(pw.version())
print(pw.PULSEWAVES_FORMAT_PLS)
```

You can also import individual classes:

```python
from pulsewaves import PulseFilter, PulseReader
```

## Read Pulses

```python
from pathlib import Path

import pulsewaves as pw

reader = pw.PulseReader()
if not reader.open(str(Path("data") / "geolas_example1.pls")):
    raise RuntimeError("could not open pulse file")

header = reader.get_header()
print(f"{header.number_of_pulses} pulses")
print(f"x: {header.min_x:.3f} to {header.max_x:.3f}")
print(f"y: {header.min_y:.3f} to {header.max_y:.3f}")
print(f"z: {header.min_z:.3f} to {header.max_z:.3f}")

while reader.read_pulse():
    pulse = reader.get_pulse()
    anchor_x, anchor_y, anchor_z = pulse.get_anchor()
    target_x, target_y, target_z = pulse.get_target()

    print(
        f"intensity={pulse.intensity} "
        f"anchor=({anchor_x:.3f}, {anchor_y:.3f}, {anchor_z:.3f}) "
        f"target=({target_x:.3f}, {target_y:.3f}, {target_z:.3f})"
    )
    break

reader.close()
```

## Reuse C++ Reader Arguments

`PulseReader.open_with_args()` accepts the same argument strings as
`PULSEreadOpener::parse()` in the C++ library. That keeps file selection,
filters, clipping, and transforms on the same code path as the native reader
logic.

```python
import pulsewaves as pw

reader = pw.PulseReader()
if not reader.open_with_args([
    "-i", "data/test.pls",
    "-keep_intensity", "1", "255",
    "-translate_intensity", "10",
]):
    raise RuntimeError("could not open pulse file")

while reader.read_pulse():
    pulse = reader.get_pulse()
    print(pulse.intensity)

reader.close()
```

## Filter Pulses

`PulseFilter.parse()` also accepts the C++ filter syntax directly. The C++
filter predicate returns `True` when a pulse should be filtered out, so keep
pulses that return `False`.

```python
import pulsewaves as pw

filter_ = pw.PulseFilter()
if not filter_.parse(["-keep_intensity", "25", "255"]):
    raise ValueError("invalid filter arguments")

reader = pw.PulseReader()
reader.open("data/test.pls")

kept = 0
while reader.read_pulse():
    pulse = reader.get_pulse()
    if not filter_.filter(pulse):
        kept += 1

reader.close()
print(f"kept {kept} pulses")
```

For geometry filters that are easier to express directly, call the bound helper
methods:

```python
filter_ = pw.PulseFilter()
filter_.addKeepCircle(600000.0, 4000000.0, 250.0)
filter_.addKeepBox(599500.0, 3999500.0, 600500.0, 4000500.0)
```

## Transform Pulses

```python
import pulsewaves as pw

pulse = pw.Pulse()
pulse.intensity = 42

transform = pw.PulseTransform()
if not transform.parse(["-translate_intensity", "10"]):
    raise ValueError("invalid transform arguments")

transform.transform(pulse)
print(pulse.intensity)
```

`PulseReader.open_with_args()` can apply transform arguments while reading, so
use an explicit `PulseTransform` only when you already have a pulse object you
want to mutate.

## Build Histograms

```python
import pulsewaves as pw

histogram = pw.PulseHistogram()
histogram.histo("intensity", 1.0)
histogram.histo("classification", 1.0)

reader = pw.PulseReader()
reader.open("data/test.pls")

while reader.read_pulse():
    histogram.add(reader.get_pulse())

reader.close()
```

The C++ `PULSEhistogram` type reports directly to `FILE*`, so the current Python
binding supports building histogram state but does not yet expose a Pythonic
`report()` result.

## Build an Occupancy Grid

```python
import pulsewaves as pw

grid = pw.PulseOccupancyGrid(1.0)

reader = pw.PulseReader()
reader.open("data/test.pls")

while reader.read_pulse():
    grid.add_pulse(reader.get_pulse())

reader.close()

print(grid.get_num_occupied())
grid.write_asc_grid("/tmp/pulse_occupancy.asc")
```

`PulseOccupancyGrid.add(x, y)` is also available when you already have integer
grid coordinates:

```python
grid = pw.PulseOccupancyGrid(5.0)
grid.add(10, 20)
grid.add(10, 21)
print(grid.occupied(10, 20))
```

## Create Header Metadata

```python
import pulsewaves as pw

sampling = pw.PulseSampling()
sampling.description = "returning waveform"
sampling.bits_per_sample = 8
sampling.number_of_samples = 96

composition = pw.PulseComposition()
composition.number_of_samplings = 1
composition.sample_units = 1

scanner = pw.PulseScanner()
scanner.instrument = "airborne waveform scanner"
scanner.serial = "demo-001"
scanner.wave_length = 1064
scanner.pulse_frequency = 100000

header = pw.PulseHeader()
header.add_scanner(scanner, 1)
header.add_descriptor(composition, [sampling], 1)
header.set_bounding_box(0.0, 1000.0, 0.0, 1000.0, 0.0, 200.0)

print(header.get_scanner(1).instrument)
print(header.get_descriptor_samplings(1)[0].description)
```

## Quantize Coordinates

`PulseQuantizer` exposes the same scale and offset fields as the underlying C++
type:

```python
import pulsewaves as pw

quantizer = pw.PulseQuantizer()
quantizer.x_scale_factor = 0.01
quantizer.x_offset = 500000.0

raw_x = quantizer.get_X(500123.45)
x = quantizer.get_x(raw_x)

print(raw_x, x)
```

## Wrapped Types

The extension wraps these PulseWaves classes:

- `PulseReader`, `PulseWriter`
- `Pulse`, `PulseHeader`, `PulseQuantizer`
- `PulseItem`, `PulseAttribute`, `PulseAttributer`
- `PulseSampling`, `PulseComposition`, `PulseDescriptor`
- `PulseVLR`, `PulseAVLR`
- `PulseScanner`, `PulseLookupTable`, `PulseTable`
- `PulseGeoKeys`, `PulseKeyEntry`
- `PulseZip`
- `PulseInventory`, `PulseSummary`, `PulseBin`, `PulseHistogram`,
  `PulseOccupancyGrid`
- `PulseFilter`, `PulseTransform`, `PulseIndex`

The module also exports PulseWaves format, compression, and extra-attribute
constants from `pulsewavesdefinitions.hpp`.

## Not Exposed

The C++ code still contains methods that work directly with `FILE*`,
`ByteStreamIn*`, and `ByteStreamOut*`. Those raw pointer APIs are intentionally
not bound; Python call sites should go through `PulseReader`, `PulseWriter`,
ordinary file paths, and Python-owned `bytes` instead.

## Tests

Run the Python smoke tests from the repository root:

```bash
PYTHONPATH=python python -m unittest discover -s tests -v
```
