from setuptools import setup, Extension
from pathlib import Path
import pybind11

# Read the README
readme_file = Path(__file__).parent.parent / "README.txt"
if readme_file.exists():
    long_description = readme_file.read_text()
else:
    long_description = "PulseWaves Python bindings for full waveform LiDAR data"

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

setup(
    name="pulsewaves",
    version="0.1.0",
    author="PulseWaves Contributors",
    description="Python bindings for PulseWaves full waveform LiDAR library",
    long_description=long_description,
    long_description_content_type="text/plain",
    packages=["pulsewaves"],
    package_data={"pulsewaves": ["*.pyi", "py.typed"]},
    ext_modules=ext_modules,
    python_requires=">=3.9",
    install_requires=[],
    extras_require={
        "dev": ["pytest>=6.0", "black", "mypy", "pybind11>=3.0.0"],
    },
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "Topic :: Scientific/Engineering :: GIS",
        "License :: OSI Approved :: GNU Lesser General Public License v2 or later (LGPLv2+)",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Programming Language :: Python :: 3.13",
        "Programming Language :: Python :: 3.14",
    ],
)
