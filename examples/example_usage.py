#!/usr/bin/env python3
"""Example usage of the native PulseWaves Python bindings."""

import sys
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(REPO_ROOT / "python"))


from pulsewaves import PulseReader, PulseFilter, PulseHistogram, version


def main():
    print(f"Using {version()}")

    reader = PulseReader()
    if not reader.open(str(REPO_ROOT / "data" / "test.pls")):
        raise SystemExit("Could not open data/test.pls")

    header = reader.get_header()
    print(f"Pulses: {header.number_of_pulses}")
    print(
        "Bounds: "
        f"X=[{header.min_x:.3f}, {header.max_x:.3f}] "
        f"Y=[{header.min_y:.3f}, {header.max_y:.3f}] "
        f"Z=[{header.min_z:.3f}, {header.max_z:.3f}]"
    )

    histogram = PulseHistogram()
    histogram.histo("intensity", 1.0)

    filter_ = PulseFilter()
    filter_.parse(["-keep_intensity", "0", "255"])

    count = 0
    while reader.read_pulse():
        pulse = reader.get_pulse()
        if not filter_.filter(pulse):
            histogram.add(pulse)
            count += 1
        if count == 10:
            break

    reader.close()
    print(f"Read {count} filtered pulses")


if __name__ == "__main__":
    main()
