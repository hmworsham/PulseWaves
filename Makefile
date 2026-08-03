all:
	cd src && make
	cd bin && make
#	cd test && make

clean: # Quick clean - use when you want to rebuild
	cd src && make clean
	cd bin && make clean
	rm -f pulseinfo pulse2pulse

clobber: # Deep clean - use before git commit or distribution
	cd src && make clobber
	cd bin && make clean
	rm -f pulseinfo pulse2pulse
	rm -f lib/*
	find python -name "*.so" -delete
	find python -name "*.dylib" -delete
	find python -name "*.pyc" -delete
	find python -name "*.pyo" -delete
	rm -rf python/__pycache__
	rm -rf python/*.egg-info
	rm -rf python/build
	rm -rf python/dist

tools:
	cd src && make
	cd bin && make
	cp bin/pulseinfo bin/pulse2pulse .

python:
	@echo "Building Python bindings (requires pybind11)..."
	cd python && python3 setup.py build_ext --inplace

install-python:
	cd python && pip install -e .

.PHONY: all clean clobber tools python install-python
