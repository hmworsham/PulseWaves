"""
Setup script for PulseWaves Python bindings

Installation:
    pip install -e .

Or with native bindings (requires pybind11):
    pip install pybind11
    python setup.py build_ext --inplace
"""

from setuptools import setup, Extension
from pathlib import Path
import subprocess
import sys

# Read the README
readme_file = Path(__file__).parent.parent / "README.txt"
if readme_file.exists():
    long_description = readme_file.read_text()
else:
    long_description = "PulseWaves Python bindings for full waveform LiDAR data"

# Try to build native extension
try:
    import pybind11

    ext_modules = [
        Extension(
            "pulsewaves.pulsewaves_native",
            sources=["pulsewaves_bind.cpp"],
            include_dirs=[
                pybind11.get_include(),
                str(Path(__file__).parent.parent / "inc"),
                str(Path(__file__).parent.parent / "src"),
            ],
            library_dirs=[str(Path(__file__).parent.parent / "lib")],
            libraries=["pulsewaves"],
            language="c++",
            extra_compile_args=["-std=c++14", "-O3"],
        )
    ]
except ImportError:
    print("pybind11 not found. Skipping native extension build.")
    print("Install with: pip install pybind11")
    ext_modules = []

setup(
    name="pulsewaves",
    version="0.1.0",
    author="PulseWaves Contributors",
    description="Python bindings for PulseWaves full waveform LiDAR library",
    long_description=long_description,
    long_description_content_type="text/plain",
    packages=["pulsewaves"],
    package_dir={"pulsewaves": "."},
    ext_modules=ext_modules,
    python_requires=">=3.7",
    install_requires=[
        "numpy>=1.19.0",
    ],
    extras_require={
        "native": ["pybind11>=2.6.0"],
        "dev": ["pytest>=6.0", "black", "mypy"],
    },
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "Topic :: Scientific/Engineering :: GIS",
        "License :: OSI Approved :: GNU Lesser General Public License v2 or later (LGPLv2+)",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
    ],
)
