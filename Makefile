all:
	cd src && make
#	cd test && make

clean: # Quick clean - use when you want to rebuild
	cd src && make clean

clobber: # Deep clean - use before git commit or distribution
	cd src && make clobber
	rm -f lib/*
	find python -name "*.so" -delete
	find python -name "*.dylib" -delete
	find python -name "*.pyc" -delete
	find python -name "*.pyo" -delete
	rm -rf python/__pycache__
	rm -rf python/*.egg-info
	rm -rf python/build
	rm -rf python/dist

python:
	@echo "Building Python bindings (requires pybind11)..."
	cd src && make
	cd python && python3 setup.py build_ext --inplace

install-python:
	cd src && make
	cd python && pip install -e .

.PHONY: all clean clobber python install-python
