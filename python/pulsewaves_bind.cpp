/*
 * Python bindings for PulseWaves library using pybind11
 *
 * This provides direct access to the PulseWaves C++ API from Python.
 *
 * Build with:
 *   c++ -O3 -Wall -shared -std=c++14 -fPIC \
 *     $(python3 -m pybind11 --includes) \
 *     -I../inc pulsewaves_bind.cpp \
 *     -L../lib -lpulsewaves \
 *     -o pulsewaves$(python3-config --extension-suffix)
 */

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "pulsereader.hpp"
#include "pulsewriter.hpp"
#include "pulsepulse.hpp"
#include "waveswaves.hpp"

namespace py = pybind11;

class PulseReaderWrapper {
private:
    PULSEreader* reader;

public:
    PulseReaderWrapper() : reader(nullptr) {}

    ~PulseReaderWrapper() {
        close();
    }

    bool open(const std::string& filename) {
        reader = new PULSEreader();
        if (!reader->open(filename.c_str())) {
            delete reader;
            reader = nullptr;
            return false;
        }
        return true;
    }

    void close() {
        if (reader) {
            reader->close();
            delete reader;
            reader = nullptr;
        }
    }

    bool read_pulse() {
        if (!reader) return false;
        return reader->read_pulse();
    }

    py::dict get_pulse() {
        if (!reader || !reader->pulse) {
            return py::dict();
        }

        PULSEpulse* p = reader->pulse;
        py::dict result;

        result["x"] = p->get_x();
        result["y"] = p->get_y();
        result["z"] = p->get_z();
        result["intensity"] = p->intensity;
        result["offset"] = p->offset;
        result["anchor_x"] = p->anchor_x;
        result["anchor_y"] = p->anchor_y;
        result["anchor_z"] = p->anchor_z;
        result["target_x"] = p->target_x;
        result["target_y"] = p->target_y;
        result["target_z"] = p->target_z;
        result["first_returning_sample"] = p->first_returning_sample;
        result["last_returning_sample"] = p->last_returning_sample;
        result["descriptor_index"] = p->descriptor_index;

        return result;
    }

    py::dict get_header() {
        if (!reader || !reader->header) {
            return py::dict();
        }

        PULSEheader* h = reader->header;
        py::dict result;

        result["number_of_pulses"] = (long long)h->number_of_pulses;
        result["number_of_pulses_by_return"] = py::list();

        for (int i = 0; i < 8; i++) {
            result["number_of_pulses_by_return"].cast<py::list>().append(
                (long long)h->number_of_pulses_by_return[i]
            );
        }

        result["min_x"] = h->min_x;
        result["max_x"] = h->max_x;
        result["min_y"] = h->min_y;
        result["max_y"] = h->max_y;
        result["min_z"] = h->min_z;
        result["max_z"] = h->max_z;

        return result;
    }

    py::array_t<float> get_waveform() {
        if (!reader || !reader->waves) {
            return py::array_t<float>(0);
        }

        WAVESwaves* w = reader->waves;
        if (!w->has_samples()) {
            return py::array_t<float>(0);
        }

        std::vector<float> samples;
        for (int i = 0; i < w->get_number_of_segments(); i++) {
            int num_samples = w->get_number_of_samples(i);
            for (int j = 0; j < num_samples; j++) {
                samples.push_back((float)w->get_sample(i, j));
            }
        }

        return py::array_t<float>(samples.size(), samples.data());
    }

    long long get_npoints() {
        if (!reader || !reader->header) return 0;
        return reader->header->number_of_pulses;
    }
};


PYBIND11_MODULE(pulsewaves_native, m) {
    m.doc() = "PulseWaves native Python bindings";

    py::class_<PulseReaderWrapper>(m, "PulseReader")
        .def(py::init<>())
        .def("open", &PulseReaderWrapper::open,
             "Open a pulse file for reading",
             py::arg("filename"))
        .def("close", &PulseReaderWrapper::close,
             "Close the pulse file")
        .def("read_pulse", &PulseReaderWrapper::read_pulse,
             "Read the next pulse from the file")
        .def("get_pulse", &PulseReaderWrapper::get_pulse,
             "Get the current pulse as a dictionary")
        .def("get_header", &PulseReaderWrapper::get_header,
             "Get the pulse file header information")
        .def("get_waveform", &PulseReaderWrapper::get_waveform,
             "Get the waveform samples for the current pulse as a numpy array")
        .def("get_npoints", &PulseReaderWrapper::get_npoints,
             "Get the total number of pulses in the file");

    m.def("version", []() {
        return "PulseWaves Python bindings v1.0";
    });
}
