# PulseWaves Python Quick Start

Your PulseWaves repository now has usable command-line tools and Python bindings!

## ✅ What's Been Set Up

### 1. Native macOS Command-Line Tools
Two compiled executables are ready to use:

```bash
./pulseinfo -i yourfile.pls
./pulse2pulse -i input.pls -o output.txt
```

Located in the repository root.

### 2. Python Bindings (Two Tiers)

#### Tier 1: High-Level Interface (Ready Now)
Subprocess wrappers around the CLI tools - works immediately:

```python
import sys
sys.path.append('/Users/hmworsham/Repos/PulseWaves/python')

from pulsewaves import pulseinfo, pulse2pulse

info = pulseinfo("file.pls", verbose=True)
pulse2pulse("input.pls", "output.txt")
```

#### Tier 2: Native C++ Bindings (Optional)
For direct library access - requires building:

```bash
cd /Users/hmworsham/Repos/PulseWaves/python
pip install pybind11 numpy
python setup.py build_ext --inplace
```

Then:
```python
from pulsewaves.pulsewaves_native import PulseReader

reader = PulseReader()
reader.open("file.pls")
header = reader.get_header()
# etc...
```

## 🚀 Quick Usage

### Add to PATH (Recommended)

```bash
# Add to your ~/.zshrc:
echo 'export PATH="$PATH:/Users/hmworsham/Repos/PulseWaves"' >> ~/.zshrc
source ~/.zshrc

# Now use anywhere:
pulseinfo -i mydata.pls
```

### Python Setup

```bash
# Option 1: Add to PYTHONPATH
echo 'export PYTHONPATH="$PYTHONPATH:/Users/hmworsham/Repos/PulseWaves/python"' >> ~/.zshrc
source ~/.zshrc

# Option 2: Install as package
cd /Users/hmworsham/Repos/PulseWaves/python
pip install -e .
```

## 📝 Example Python Script

Save as `process_pulse.py`:

```python
#!/usr/bin/env python3
import sys
sys.path.append('/Users/hmworsham/Repos/PulseWaves/python')

from pulsewaves import pulseinfo, pulse2pulse

# Get file info
info = pulseinfo("data.pls", verbose=True)
print("File information:")
print(info['output'])

# Convert to text format
print("\nConverting to text...")
pulse2pulse("data.pls", "output.txt", verbose=True)
print("Done!")
```

Run it:
```bash
python3 process_pulse.py
```

## 🔧 File Structure

```
PulseWaves/
├── pulseinfo          # ← CLI tool (macOS binary)
├── pulse2pulse        # ← CLI tool (macOS binary)
│
├── python/            # ← Python package
│   ├── __init__.py
│   ├── pulsewaves.py  # High-level interface
│   ├── pulsewaves_bind.cpp  # Native bindings source
│   └── setup.py
│
├── bin/               # ← Source code for CLI tools
│   ├── pulseinfo.cpp
│   ├── pulse2pulse.cpp
│   └── Makefile
│
├── examples/
│   └── example_usage.py
│
├── lib/
│   └── libpulsewaves.a  # Compiled library
│
├── src/               # Library source
├── inc/               # Headers
│
└── Documentation:
    ├── SETUP.md       # Detailed setup guide
    ├── PYTHON_QUICKSTART.md  # This file
    └── python/README.md  # Python API reference
```

## 🎯 What to Do Next

1. **Test the tools** with your pulse data files:
   ```bash
   ./pulseinfo -i your_file.pls -verbose
   ```

2. **Try the Python interface**:
   ```python
   from pulsewaves import pulseinfo
   info = pulseinfo("your_file.pls")
   ```

3. **Build native bindings** (optional, for better performance):
   ```bash
   pip install pybind11 numpy
   cd python && python setup.py build_ext --inplace
   ```

4. **Add to PATH** for convenience (see above)

## 📚 More Information

- **Detailed Setup**: See [SETUP.md](SETUP.md)
- **Python API**: See [python/README.md](python/README.md)
- **Examples**: Run `python3 examples/example_usage.py`

## ❓ Troubleshooting

**"Command not found"**
→ Either use `./pulseinfo` or add to PATH

**"Module not found"**
→ Add python/ to PYTHONPATH or `pip install -e python/`

**Native bindings won't build**
→ High-level interface works fine without them!

## 🎉 Summary

You now have:
- ✅ Native macOS executables for `pulseinfo` and `pulse2pulse`
- ✅ Python wrappers ready to use
- ✅ Optional native C++ bindings (buildable)
- ✅ All documentation and examples

The 32-bit Windows `.exe` files are no longer needed - you have native Mac tools!
