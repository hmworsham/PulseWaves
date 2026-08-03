"""
PulseWaves Python Package

Provides Python access to PulseWaves full waveform LiDAR data.

Two interfaces are available:
1. High-level Python API using subprocess calls to compiled tools
2. Native bindings (if pybind11 bindings are compiled)

Example usage:
    from pulsewaves import pulseinfo, pulse2pulse

    # Get file information
    info = pulseinfo("data.pls")
    print(info)

    # Convert formats
    pulse2pulse("input.pls", "output.txt")

    # Or use the native interface (if compiled):
    from pulsewaves.native import PulseReader

    with PulseReader("data.pls") as reader:
        for pulse in reader:
            print(pulse)
"""

from .pulsewaves import (
    pulseinfo,
    pulse2pulse,
    PulseData,
    PulseReader,
    PulseWriter,
)

__version__ = "0.1.0"
__all__ = [
    "pulseinfo",
    "pulse2pulse",
    "PulseData",
    "PulseReader",
    "PulseWriter",
]

# Try to import native bindings if available
try:
    from . import pulsewaves_native
    __all__.append("pulsewaves_native")
    HAS_NATIVE = True
except ImportError:
    HAS_NATIVE = False
