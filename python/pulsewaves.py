"""
Python bindings for PulseWaves library using ctypes.

This module provides Python access to the compiled PulseWaves C++ library,
allowing you to read and write pulse waveform data.

Example usage:
    from pulsewaves import PulseReader

    reader = PulseReader("input.pls")
    for pulse in reader:
        print(f"Pulse at ({pulse.x}, {pulse.y}, {pulse.z})")
"""

import ctypes
import os
from pathlib import Path
from typing import Optional, Iterator
import numpy as np

# Find the library
LIB_PATH = Path(__file__).parent.parent / "lib" / "libpulsewaves.a"
if not LIB_PATH.exists():
    raise FileNotFoundError(f"PulseWaves library not found at {LIB_PATH}. Run 'make' in src/ first.")


class PulseData(ctypes.Structure):
    """Structure representing a single pulse."""
    _fields_ = [
        ("x", ctypes.c_double),
        ("y", ctypes.c_double),
        ("z", ctypes.c_double),
        ("intensity", ctypes.c_uint16),
        ("offset", ctypes.c_int64),
        ("anchor_x", ctypes.c_int32),
        ("anchor_y", ctypes.c_int32),
        ("anchor_z", ctypes.c_int32),
        ("target_x", ctypes.c_int32),
        ("target_y", ctypes.c_int32),
        ("target_z", ctypes.c_int32),
        ("first_returning_sample", ctypes.c_int16),
        ("last_returning_sample", ctypes.c_int16),
        ("descriptor_index", ctypes.c_uint8),
        ("reserved", ctypes.c_uint8),
        ("edge_of_scan_line", ctypes.c_uint8),
        ("scan_direction", ctypes.c_uint8),
        ("facet", ctypes.c_uint8),
        ("intensity_is_valid", ctypes.c_uint8),
    ]


class PulseReader:
    """
    High-level Python interface for reading PulseWaves files.

    Since the C++ library uses object-oriented design, we'll need to create
    wrapper executables or use a proper binding like pybind11 for full functionality.

    For now, this serves as a template and you should use the command-line tools.
    """

    def __init__(self, filename: str):
        self.filename = filename
        if not os.path.exists(filename):
            raise FileNotFoundError(f"Pulse file not found: {filename}")

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        pass

    def read_header(self) -> dict:
        """Read pulse file header information."""
        raise NotImplementedError(
            "Direct Python bindings require pybind11. "
            "Use the CLI tools in bin/ or build proper bindings."
        )


class PulseWriter:
    """High-level Python interface for writing PulseWaves files."""

    def __init__(self, filename: str):
        self.filename = filename

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        pass

    def write_pulse(self, pulse: PulseData) -> None:
        """Write a single pulse to the file."""
        raise NotImplementedError(
            "Direct Python bindings require pybind11. "
            "Use the CLI tools in bin/ or build proper bindings."
        )


def pulseinfo(filename: str, verbose: bool = False) -> dict:
    """
    Get information about a pulse file.

    This is a Python wrapper around the pulseinfo command-line tool.

    Args:
        filename: Path to the pulse file
        verbose: Whether to include verbose output

    Returns:
        Dictionary containing pulse file information
    """
    import subprocess

    bin_dir = Path(__file__).parent.parent / "bin"
    pulseinfo_exe = bin_dir / "pulseinfo"

    if not pulseinfo_exe.exists():
        raise FileNotFoundError(
            f"pulseinfo executable not found at {pulseinfo_exe}. "
            "Run 'make tools' to build the command-line tools."
        )

    cmd = [str(pulseinfo_exe), "-i", filename]
    if verbose:
        cmd.append("-verbose")

    result = subprocess.run(cmd, capture_output=True, text=True)

    if result.returncode != 0:
        raise RuntimeError(f"pulseinfo failed: {result.stderr}")

    return {"output": result.stdout, "raw": result.stdout}


def pulse2pulse(input_file: str, output_file: str, **kwargs) -> None:
    """
    Convert pulse file formats.

    This is a Python wrapper around the pulse2pulse command-line tool.

    Args:
        input_file: Input pulse file path
        output_file: Output pulse file path
        **kwargs: Additional arguments to pass to pulse2pulse
    """
    import subprocess

    bin_dir = Path(__file__).parent.parent / "bin"
    pulse2pulse_exe = bin_dir / "pulse2pulse"

    if not pulse2pulse_exe.exists():
        raise FileNotFoundError(
            f"pulse2pulse executable not found at {pulse2pulse_exe}. "
            "Run 'make tools' to build the command-line tools."
        )

    cmd = [str(pulse2pulse_exe), "-i", input_file, "-o", output_file]

    # Add additional keyword arguments
    for key, value in kwargs.items():
        cmd.append(f"-{key}")
        if value is not True:
            cmd.append(str(value))

    result = subprocess.run(cmd, capture_output=True, text=True)

    if result.returncode != 0:
        raise RuntimeError(f"pulse2pulse failed: {result.stderr}")


__all__ = [
    "PulseData",
    "PulseReader",
    "PulseWriter",
    "pulseinfo",
    "pulse2pulse",
]
