#!/usr/bin/env python3
"""
Example usage of PulseWaves Python bindings

This demonstrates both the high-level and native interfaces.
"""

import sys
import os

# Add parent directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

def example_high_level():
    """Example using high-level Python interface (works without native bindings)"""
    print("=" * 60)
    print("Example 1: High-Level Interface")
    print("=" * 60)

    from pulsewaves import pulseinfo, pulse2pulse

    # This would work if you have a pulse file:
    # info = pulseinfo("your_file.pls", verbose=True)
    # print(info['output'])

    print("✓ High-level interface imported successfully")
    print("  Usage: pulseinfo('file.pls', verbose=True)")
    print("  Usage: pulse2pulse('input.pls', 'output.txt')")
    print()


def example_native():
    """Example using native C++ bindings (requires compilation)"""
    print("=" * 60)
    print("Example 2: Native Interface (if available)")
    print("=" * 60)

    try:
        from pulsewaves.pulsewaves_native import PulseReader
        import numpy as np

        print("✓ Native bindings are available!")
        print()
        print("Example code:")
        print("""
    reader = PulseReader()
    if reader.open("data.pls"):
        header = reader.get_header()
        print(f"Pulses: {header['number_of_pulses']}")

        while reader.read_pulse():
            pulse = reader.get_pulse()
            waveform = reader.get_waveform()  # numpy array
            print(f"Position: ({pulse['x']}, {pulse['y']}, {pulse['z']})")

        reader.close()
        """)

    except ImportError as e:
        print("⚠ Native bindings not available")
        print(f"  Error: {e}")
        print()
        print("To build native bindings:")
        print("  1. pip install pybind11 numpy")
        print("  2. cd python && python setup.py build_ext --inplace")
        print()
        print("The high-level interface works without native bindings.")

    print()


def example_cli_tools():
    """Example using command-line tools"""
    print("=" * 60)
    print("Example 3: Command-Line Tools")
    print("=" * 60)

    print("Available tools:")
    print("  bin/pulseinfo -i file.pls [-verbose]")
    print("  bin/pulse2pulse -i input.pls -o output.txt [-verbose]")
    print()

    print("Testing tools...")
    repo_root = os.path.dirname(os.path.dirname(__file__))

    # Test pulseinfo
    pulseinfo_path = os.path.join(repo_root, 'bin', 'pulseinfo')
    if os.path.exists(pulseinfo_path):
        print(f"✓ pulseinfo found at: {pulseinfo_path}")
    else:
        print(f"⚠ pulseinfo not found (expected at {pulseinfo_path})")

    # Test pulse2pulse
    pulse2pulse_path = os.path.join(repo_root, 'bin', 'pulse2pulse')
    if os.path.exists(pulse2pulse_path):
        print(f"✓ pulse2pulse found at: {pulse2pulse_path}")
    else:
        print(f"⚠ pulse2pulse not found (expected at {pulse2pulse_path})")

    print()


if __name__ == "__main__":
    print("\n" + "=" * 60)
    print("PulseWaves Python Bindings - Usage Examples")
    print("=" * 60)
    print()

    example_high_level()
    example_native()
    example_cli_tools()

    print("=" * 60)
    print("For more information, see:")
    print("  - README.md (main setup guide)")
    print("  - python/README.md (Python API reference)")
    print("=" * 60)
    print()
