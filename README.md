# PulseWaves

PulseWaves is a waveform LiDAR format and C++ library originally developed by
Martin Isenburg / rapidlasso GmbH. This fork keeps the C++ core in `inc/` and
`src/`, and exposes it to Python through one native `pybind11` extension.

There is no subprocess wrapper layer and no separate command-line tool layer:
Python imports `pulsewaves_native` and calls the C++ library in-process.

## Repository Layout

```text
PulseWaves/
├── inc/                 Public C++ headers
├── src/                 PulseWaves C++ implementation
├── lib/                 Built libpulsewaves.a
├── python/              pybind11 extension and Python package
├── tests/               Python smoke tests
├── examples/            Python examples
├── data/                Small sample PLS files
├── Makefile             Top-level C++/Python build shortcuts
└── setup.sh             Convenience editable-install helper
```

## Build

Build the static C++ library first:

```bash
make
```

Then install the Python bindings from `python/`:

```bash
cd python
python -m pip install -e .
```

For in-place extension builds during local development:

```bash
make python
export PYTHONPATH="$PWD/python:$PYTHONPATH"
```

The Python package declares its build requirements in
[python/pyproject.toml](python/pyproject.toml), including `pybind11`.

## Quick Example

```python
from pulsewaves import PulseFilter, PulseHistogram, PulseReader

reader = PulseReader()
if not reader.open("data/test.pls"):
    raise RuntimeError("could not open data/test.pls")

header = reader.get_header()
print(f"pulses: {header.number_of_pulses}")
print(f"x bounds: {header.min_x:.3f} to {header.max_x:.3f}")

filter_ = PulseFilter()
filter_.parse(["-keep_intensity", "0", "255"])

histogram = PulseHistogram()
histogram.histo("intensity", 1.0)

kept = 0
while reader.read_pulse():
    pulse = reader.get_pulse()
    if not filter_.filter(pulse):
        histogram.add(pulse)
        kept += 1

reader.close()
print(f"kept {kept} pulses")
```

## Reader Options

`PulseReader.open(path)` opens a single file directly. `PulseReader.open_with_args`
uses the C++ `PULSEreadOpener` parser, so file-oriented options such as `-i`,
clipping, filtering, and transforms can be reused from Python:

```python
from pulsewaves import PulseReader

reader = PulseReader()
ok = reader.open_with_args([
    "-i", "data/test.pls",
    "-inside_tile", "600000", "4000000", "1000",
    "-keep_intensity", "25", "255",
])

if not ok:
    raise RuntimeError("could not open filtered reader")

while reader.read_pulse():
    pulse = reader.get_pulse()
    print(pulse.intensity, pulse.get_anchor())

reader.close()
```

## Metadata Example

The binding exposes PulseWaves metadata records as Python classes with readable
and writable fields:

```python
from pulsewaves import PulseComposition, PulseHeader, PulseSampling, PulseScanner

sampling = PulseSampling()
sampling.description = "returning waveform"
sampling.bits_per_sample = 8
sampling.number_of_samples = 80

composition = PulseComposition()
composition.number_of_samplings = 1

scanner = PulseScanner()
scanner.instrument = "LVIS"
scanner.wave_length = 1064

header = PulseHeader()
header.add_scanner(scanner, 1)
header.add_descriptor(composition, [sampling], 1)
header.set_bounding_box(0, 10, 0, 10, 0, 100)

print(header.get_scanner(1).instrument)
print(header.get_descriptor_composition(1).number_of_samplings)
```

## Exposed API

The Python module wraps the core PulseWaves data model and utility types:

- File I/O: `PulseReader`, `PulseWriter`
- Pulse data: `Pulse`, `PulseHeader`, `PulseQuantizer`
- Descriptors: `PulseSampling`, `PulseComposition`, `PulseDescriptor`
- Variable-length records: `PulseVLR`, `PulseAVLR`
- Metadata tables: `PulseScanner`, `PulseLookupTable`, `PulseTable`,
  `PulseGeoKeys`, `PulseKeyEntry`
- Compression descriptors: `PulseZip`, `PulseItem`
- Analysis utilities: `PulseFilter`, `PulseTransform`, `PulseHistogram`,
  `PulseBin`, `PulseOccupancyGrid`, `PulseInventory`, `PulseSummary`,
  `PulseIndex`

Raw `FILE*`, `ByteStreamIn*`, and `ByteStreamOut*` methods are intentionally
left out of the Python surface. Python code should use `PulseReader`,
`PulseWriter`, `bytes`, and ordinary path strings instead of passing C pointers
across the boundary.

## Supported Formats

Supported input formats depend on the readers compiled into `src/`:

- `.pls`: PulseWaves, uncompressed
- `.plz`: PulseWaves, compressed
- `.lgw`: Leica Geosystems waveform data
- `.lgc`: Leica Geosystems waveform data, compressed
- `.gcw`: GeoCoding Waveform

The current writer layer exposes:

- `.pls`: PulseWaves, uncompressed
- `.txt`: ASCII text

## Development

Run the Python smoke tests from the repository root:

```bash
PYTHONPATH=python python -m unittest discover -s tests -v
```

If `pulsewaves_native` has not been built for the active interpreter, the native
binding tests skip themselves.

Clean C++ object files:

```bash
make clean
```

Remove generated libraries, Python extension modules, and Python build
directories:

```bash
make clobber
```

## License

PulseWaves is available under the GNU Lesser General Public License v2 or
later. See [COPYING.txt](COPYING.txt).
