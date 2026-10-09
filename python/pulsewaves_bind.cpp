/*
 * Native Python bindings for the PulseWaves C++ API.
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstdlib>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "pulsedescriptor.hpp"
#include "pulsefilter.hpp"
#include "pulseheader.hpp"
#include "pulseindex.hpp"
#include "pulsepulse.hpp"
#include "pulsereader.hpp"
#include "pulseutility.hpp"
#include "pulsevlrs.hpp"
#include "pulsewavesdefinitions.hpp"
#include "pulsewriter.hpp"
#include "pulsezip.hpp"
#include "pulsetransform.hpp"

namespace py = pybind11;

template <std::size_t N>
std::string fixed_string(const CHAR (&value)[N])
{
    return std::string(value, strnlen(value, N));
}

template <std::size_t N>
void set_fixed_string(CHAR (&target)[N], const std::string& value)
{
    std::memset(target, 0, N);
    std::strncpy(target, value.c_str(), N);
}

template <std::size_t N>
std::vector<U8> fixed_bytes(const U8 (&value)[N])
{
    return std::vector<U8>(value, value + N);
}

template <std::size_t N>
void set_fixed_bytes(U8 (&target)[N], const std::vector<U8>& value)
{
    if (value.size() != N) {
        throw std::invalid_argument("byte array has the wrong length");
    }
    std::memcpy(target, value.data(), N);
}

std::vector<char*> argv_from_args(std::vector<std::string>& args)
{
    std::vector<char*> argv;
    argv.reserve(args.size() + 1);
    argv.push_back(const_cast<char*>("pulsewaves"));
    for (std::string& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    return argv;
}

class PulseReaderWrapper {
public:
    bool open(const std::string& filename)
    {
        close();
        PULSEreadOpener opener;
        opener.set_file_name(filename.c_str());
        reader.reset(opener.open());
        return static_cast<bool>(reader);
    }

    bool open_with_args(const std::vector<std::string>& arguments)
    {
        close();

        std::vector<std::string> mutable_args = arguments;
        std::vector<char*> argv = argv_from_args(mutable_args);

        PULSEreadOpener opener;
        if (!opener.parse(static_cast<int>(argv.size()), argv.data())) {
            return false;
        }

        reader.reset(opener.open());
        return static_cast<bool>(reader);
    }

    void close()
    {
        if (reader) {
            reader->close();
            reader.reset();
        }
    }

    bool read_pulse()
    {
        if (!reader || !reader->read_pulse()) {
            return false;
        }

        reader->pulse.compute_anchor_and_target_and_dir();
        return true;
    }

    bool read_waves()
    {
        return reader && reader->read_waves();
    }

    bool seek(I64 index)
    {
        return reader && reader->seek(index);
    }

    I32 get_format() const
    {
        if (!reader) {
            throw std::runtime_error("reader is not open");
        }
        return reader->get_format();
    }

    I64 get_n_pulses() const
    {
        if (!reader) {
            throw std::runtime_error("reader is not open");
        }
        return reader->npulses;
    }

    PULSEheader& get_header()
    {
        if (!reader) {
            throw std::runtime_error("reader is not open");
        }
        return reader->header;
    }

    PULSEpulse& get_pulse()
    {
        if (!reader) {
            throw std::runtime_error("reader is not open");
        }
        return reader->pulse;
    }

private:
    struct ReaderDeleter {
        void operator()(PULSEreader* value) const
        {
            if (value) {
                value->close();
                delete value;
            }
        }
    };

    std::unique_ptr<PULSEreader, ReaderDeleter> reader;
};

class PulseWriterWrapper {
public:
    bool open(const std::string& filename, PULSEheader& header)
    {
        close();
        PULSEwriteOpener opener;
        opener.set_file_name(filename.c_str());
        writer.reset(opener.open(&header));
        return static_cast<bool>(writer);
    }

    bool open_with_args(const std::vector<std::string>& arguments, PULSEheader& header)
    {
        close();

        std::vector<std::string> mutable_args = arguments;
        std::vector<char*> argv = argv_from_args(mutable_args);

        PULSEwriteOpener opener;
        if (!opener.parse(static_cast<int>(argv.size()), argv.data())) {
            return false;
        }

        writer.reset(opener.open(&header));
        return static_cast<bool>(writer);
    }

    void close(bool update_npulses=true)
    {
        if (writer) {
            writer->close(update_npulses);
            writer.reset();
        }
    }

    bool write_pulse(const PULSEpulse& pulse)
    {
        return writer && writer->write_pulse(&pulse);
    }

    bool update_header(const PULSEheader& header, bool use_inventory=true, bool update_extra_bytes=false)
    {
        return writer && writer->update_header(&header, use_inventory, update_extra_bytes);
    }

    I64 get_current_offset() const
    {
        if (!writer) {
            throw std::runtime_error("writer is not open");
        }
        return writer->get_current_offset();
    }

private:
    struct WriterDeleter {
        void operator()(PULSEwriter* value) const
        {
            if (value) {
                value->close();
                delete value;
            }
        }
    };

    std::unique_ptr<PULSEwriter, WriterDeleter> writer;
};

#define FIXED_STRING_PROPERTY(cls, name, member) \
    def_property(name, [](const cls& self) { return fixed_string(self.member); }, \
                 [](cls& self, const std::string& value) { set_fixed_string(self.member, value); })

py::list pulse_bin_entries(const PULSEbin& bin)
{
    py::list entries;
    for (const PULSEbin::Entry& entry : bin.snapshot()) {
        py::dict item;
        item["bin"] = entry.bin;
        item["minimum"] = entry.minimum;
        item["maximum"] = entry.maximum;
        item["count"] = entry.count;
        if (bin.has_values()) {
            item["value_sum"] = entry.value_sum;
            item["average"] = entry.average;
        } else {
            item["value_sum"] = py::none();
            item["average"] = py::none();
        }
        entries.append(item);
    }
    return entries;
}

py::dict pulse_bin_snapshot(const PULSEbin& bin)
{
    py::dict snapshot;
    snapshot["step"] = bin.step();
    snapshot["count"] = bin.get_count();
    snapshot["average"] = bin.average();
    snapshot["bins"] = pulse_bin_entries(bin);
    return snapshot;
}

void add_histogram_bin(py::dict& snapshot, const PULSEhistogram& histogram, const char* name)
{
    const PULSEbin* bin = histogram.get_bin(name);
    if (bin) {
        snapshot[name] = pulse_bin_snapshot(*bin);
    }
}

py::dict pulse_histogram_snapshot(const PULSEhistogram& histogram)
{
    py::dict snapshot;
    add_histogram_bin(snapshot, histogram, "T");
    add_histogram_bin(snapshot, histogram, "time");
    add_histogram_bin(snapshot, histogram, "offset");
    add_histogram_bin(snapshot, histogram, "anchor_x");
    add_histogram_bin(snapshot, histogram, "anchor_y");
    add_histogram_bin(snapshot, histogram, "anchor_z");
    add_histogram_bin(snapshot, histogram, "target_x");
    add_histogram_bin(snapshot, histogram, "target_y");
    add_histogram_bin(snapshot, histogram, "target_z");
    add_histogram_bin(snapshot, histogram, "descriptor");
    add_histogram_bin(snapshot, histogram, "intensity");
    add_histogram_bin(snapshot, histogram, "classification");
    add_histogram_bin(snapshot, histogram, "samples");
    add_histogram_bin(snapshot, histogram, "anchor_X");
    add_histogram_bin(snapshot, histogram, "anchor_Y");
    add_histogram_bin(snapshot, histogram, "anchor_Z");
    add_histogram_bin(snapshot, histogram, "target_X");
    add_histogram_bin(snapshot, histogram, "target_Y");
    add_histogram_bin(snapshot, histogram, "target_Z");
    return snapshot;
}

PYBIND11_MODULE(pulsewaves_native, m) {
    m.doc() = "Native PulseWaves bindings";

    m.attr("PULSEWAVES_VERSION_MAJOR") = PULSEWAVES_VERSION_MAJOR;
    m.attr("PULSEWAVES_VERSION_MINOR") = PULSEWAVES_VERSION_MINOR;
    m.attr("PULSEWAVES_REVISION") = PULSEWAVES_REVISION;
    m.attr("PULSEWAVES_BUILD_DATE") = PULSEWAVES_BUILD_DATE;
    m.attr("PULSEWAVES_UNDEFINED") = PULSEWAVES_UNDEFINED;
    m.attr("PULSEWAVES_OUTGOING") = PULSEWAVES_OUTGOING;
    m.attr("PULSEWAVES_RETURNING") = PULSEWAVES_RETURNING;
    m.attr("PULSEWAVES_UNCOMPRESSED") = PULSEWAVES_UNCOMPRESSED;
    m.attr("PULSEWAVES_FORMAT_DEFAULT") = PULSEWAVES_FORMAT_DEFAULT;
    m.attr("PULSEWAVES_FORMAT_PLS") = PULSEWAVES_FORMAT_PLS;
    m.attr("PULSEWAVES_FORMAT_PLZ") = PULSEWAVES_FORMAT_PLZ;
    m.attr("PULSEWAVES_FORMAT_LAS") = PULSEWAVES_FORMAT_LAS;
    m.attr("PULSEWAVES_FORMAT_LAZ") = PULSEWAVES_FORMAT_LAZ;
    m.attr("PULSEWAVES_FORMAT_LGW") = PULSEWAVES_FORMAT_LGW;
    m.attr("PULSEWAVES_FORMAT_GCW") = PULSEWAVES_FORMAT_GCW;
    m.attr("PULSEWAVES_FORMAT_SDF") = PULSEWAVES_FORMAT_SDF;
    m.attr("PULSEWAVES_FORMAT_CSD") = PULSEWAVES_FORMAT_CSD;
    m.attr("PULSEWAVES_FORMAT_DAT") = PULSEWAVES_FORMAT_DAT;
    m.attr("PULSEWAVES_FORMAT_TXT") = PULSEWAVES_FORMAT_TXT;
    m.attr("PULSE_EXTRA_ATTRIBUTE_U8") = PULSE_EXTRA_ATTRIBUTE_U8;
    m.attr("PULSE_EXTRA_ATTRIBUTE_I8") = PULSE_EXTRA_ATTRIBUTE_I8;
    m.attr("PULSE_EXTRA_ATTRIBUTE_U16") = PULSE_EXTRA_ATTRIBUTE_U16;
    m.attr("PULSE_EXTRA_ATTRIBUTE_I16") = PULSE_EXTRA_ATTRIBUTE_I16;
    m.attr("PULSE_EXTRA_ATTRIBUTE_U32") = PULSE_EXTRA_ATTRIBUTE_U32;
    m.attr("PULSE_EXTRA_ATTRIBUTE_I32") = PULSE_EXTRA_ATTRIBUTE_I32;
    m.attr("PULSE_EXTRA_ATTRIBUTE_U64") = PULSE_EXTRA_ATTRIBUTE_U64;
    m.attr("PULSE_EXTRA_ATTRIBUTE_I64") = PULSE_EXTRA_ATTRIBUTE_I64;
    m.attr("PULSE_EXTRA_ATTRIBUTE_F32") = PULSE_EXTRA_ATTRIBUTE_F32;
    m.attr("PULSE_EXTRA_ATTRIBUTE_F64") = PULSE_EXTRA_ATTRIBUTE_F64;
    m.attr("PULSEZIP_COMPRESSOR_NONE") = PULSEZIP_COMPRESSOR_NONE;
    m.attr("PULSEZIP_COMPRESSOR_DEFAULT") = PULSEZIP_COMPRESSOR_DEFAULT;
    m.attr("PULSEZIP_CODER_ARITHMETIC") = PULSEZIP_CODER_ARITHMETIC;
    m.attr("PULSEZIP_CHUNK_SIZE_DEFAULT") = PULSEZIP_CHUNK_SIZE_DEFAULT;

    py::class_<PULSEquantizer>(m, "PulseQuantizer")
        .def(py::init<>())
        .def_readwrite("t_scale_factor", &PULSEquantizer::t_scale_factor)
        .def_readwrite("t_offset", &PULSEquantizer::t_offset)
        .def_readwrite("x_scale_factor", &PULSEquantizer::x_scale_factor)
        .def_readwrite("y_scale_factor", &PULSEquantizer::y_scale_factor)
        .def_readwrite("z_scale_factor", &PULSEquantizer::z_scale_factor)
        .def_readwrite("x_offset", &PULSEquantizer::x_offset)
        .def_readwrite("y_offset", &PULSEquantizer::y_offset)
        .def_readwrite("z_offset", &PULSEquantizer::z_offset)
        .def("get_T", &PULSEquantizer::get_T)
        .def("get_T_msec", &PULSEquantizer::get_T_msec)
        .def("get_T_usec", &PULSEquantizer::get_T_usec)
        .def("get_T_nsec", &PULSEquantizer::get_T_nsec)
        .def("get_T_psec", &PULSEquantizer::get_T_psec)
        .def("get_t", &PULSEquantizer::get_t)
        .def("get_t_msec", &PULSEquantizer::get_t_msec)
        .def("get_t_usec", &PULSEquantizer::get_t_usec)
        .def("get_t_nsec", &PULSEquantizer::get_t_nsec)
        .def("get_t_psec", &PULSEquantizer::get_t_psec)
        .def("get_x", &PULSEquantizer::get_x)
        .def("get_y", &PULSEquantizer::get_y)
        .def("get_z", &PULSEquantizer::get_z)
        .def("get_X", &PULSEquantizer::get_X)
        .def("get_Y", &PULSEquantizer::get_Y)
        .def("get_Z", &PULSEquantizer::get_Z);

    py::enum_<PULSEitem::Type>(m, "PulseItemType")
        .value("EXTRABYTES", PULSEitem::EXTRABYTES)
        .value("PULSE0", PULSEitem::PULSE0)
        .value("PULSESOURCEID16", PULSEitem::PULSESOURCEID16)
        .value("PULSESOURCEID32", PULSEitem::PULSESOURCEID32);

    py::class_<PULSEitem>(m, "PulseItem")
        .def(py::init<>())
        .def_readwrite("type", &PULSEitem::type)
        .def_readwrite("size", &PULSEitem::size)
        .def_readwrite("version", &PULSEitem::version)
        .def("is_type", &PULSEitem::is_type)
        .def("get_name", &PULSEitem::get_name);

    py::class_<PULSEattribute>(m, "PulseAttribute")
        .def_static("from_size", [](U8 size) { return PULSEattribute(size); })
        .def_static("from_type", [](U32 type, const std::string& name, const std::string& description, U32 dim) {
            return PULSEattribute(type, name.c_str(), description.c_str(), dim);
        }, py::arg("type"), py::arg("name"), py::arg("description")="", py::arg("dim")=1)
        .def_readwrite("data_type", &PULSEattribute::data_type)
        .def_readwrite("options", &PULSEattribute::options)
        .FIXED_STRING_PROPERTY(PULSEattribute, "name", name)
        .def_readwrite("offset_to_first_byte", &PULSEattribute::offset_to_first_byte)
        .FIXED_STRING_PROPERTY(PULSEattribute, "description", description)
        .def("set_scale", &PULSEattribute::set_scale, py::arg("scale"), py::arg("dim")=0)
        .def("set_offset", &PULSEattribute::set_offset, py::arg("offset"), py::arg("dim")=0)
        .def("has_no_data", &PULSEattribute::has_no_data)
        .def("has_min", &PULSEattribute::has_min)
        .def("has_max", &PULSEattribute::has_max)
        .def("has_scale", &PULSEattribute::has_scale)
        .def("has_offset", &PULSEattribute::has_offset)
        .def("get_size", &PULSEattribute::get_size);

    py::class_<PULSEattributer>(m, "PulseAttributer")
        .def(py::init<>())
        .def_readonly("number_extra_attributes", &PULSEattributer::number_extra_attributes)
        .def("clean_extra_attributes", &PULSEattributer::clean_extra_attributes)
        .def("add_extra_attribute", &PULSEattributer::add_extra_attribute)
        .def("get_total_extra_attributes_size", &PULSEattributer::get_total_extra_attributes_size)
        .def("get_extra_attribute_index", &PULSEattributer::get_extra_attribute_index)
        .def("get_extra_attribute_array_offset_by_name",
             static_cast<I32 (PULSEattributer::*)(const char*) const>(&PULSEattributer::get_extra_attribute_array_offset))
        .def("get_extra_attribute_array_offset",
             static_cast<I32 (PULSEattributer::*)(I32) const>(&PULSEattributer::get_extra_attribute_array_offset))
        .def("remove_extra_attribute",
             static_cast<BOOL (PULSEattributer::*)(I32)>(&PULSEattributer::remove_extra_attribute))
        .def("remove_extra_attribute",
             static_cast<BOOL (PULSEattributer::*)(const char*)>(&PULSEattributer::remove_extra_attribute));

    py::class_<PULSEsampling>(m, "PulseSampling")
        .def(py::init<>())
        .def_readwrite("size", &PULSEsampling::size)
        .def_readwrite("reserved", &PULSEsampling::reserved)
        .def_readwrite("type", &PULSEsampling::type)
        .def_readwrite("channel", &PULSEsampling::channel)
        .def_readwrite("unused", &PULSEsampling::unused)
        .def_readwrite("bits_for_duration_from_anchor", &PULSEsampling::bits_for_duration_from_anchor)
        .def_readwrite("scale_for_duration_from_anchor", &PULSEsampling::scale_for_duration_from_anchor)
        .def_readwrite("offset_for_duration_from_anchor", &PULSEsampling::offset_for_duration_from_anchor)
        .def_readwrite("bits_for_number_of_segments", &PULSEsampling::bits_for_number_of_segments)
        .def_readwrite("bits_for_number_of_samples", &PULSEsampling::bits_for_number_of_samples)
        .def_readwrite("number_of_segments", &PULSEsampling::number_of_segments)
        .def_readwrite("number_of_samples", &PULSEsampling::number_of_samples)
        .def_readwrite("bits_per_sample", &PULSEsampling::bits_per_sample)
        .def_readwrite("lookup_table_index", &PULSEsampling::lookup_table_index)
        .def_readwrite("sample_units", &PULSEsampling::sample_units)
        .def_readwrite("compression", &PULSEsampling::compression)
        .FIXED_STRING_PROPERTY(PULSEsampling, "description", description)
        .def("size_of_attributes", &PULSEsampling::size_of_attributes)
        .def("is_equal", [](const PULSEsampling& self, const PULSEsampling& other) {
            return self.is_equal(&other);
        });

    py::class_<PULSEcomposition>(m, "PulseComposition")
        .def(py::init<>())
        .def_readwrite("size", &PULSEcomposition::size)
        .def_readwrite("reserved", &PULSEcomposition::reserved)
        .def_readwrite("optical_center_to_anchor_point", &PULSEcomposition::optical_center_to_anchor_point)
        .def_readwrite("number_of_extra_waves_bytes", &PULSEcomposition::number_of_extra_waves_bytes)
        .def_readwrite("number_of_samplings", &PULSEcomposition::number_of_samplings)
        .def_readwrite("scanner_index", &PULSEcomposition::scanner_index)
        .def_readwrite("sample_units", &PULSEcomposition::sample_units)
        .def_readwrite("compression", &PULSEcomposition::compression)
        .FIXED_STRING_PROPERTY(PULSEcomposition, "description", description)
        .def("size_of_attributes", &PULSEcomposition::size_of_attributes)
        .def("is_equal", [](const PULSEcomposition& self, const PULSEcomposition& other) {
            return self.is_equal(&other);
        });

    py::class_<PULSEdescriptor>(m, "PulseDescriptor")
        .def(py::init<>())
        .def("is_equal_descriptor", [](const PULSEdescriptor& self, const PULSEdescriptor& other) {
            return self.is_equal(&other);
        })
        .def("is_equal", [](const PULSEdescriptor& self, const PULSEcomposition& composition,
                            const std::vector<PULSEsampling>& samplings) {
            return self.is_equal(&composition, samplings.data());
        });

    py::class_<PULSEvlr>(m, "PulseVLR")
        .def(py::init<>())
        .def(py::init([](const std::string& user_id, U32 record_id, const std::string& description) {
            return PULSEvlr(user_id.c_str(), record_id, description.c_str());
        }), py::arg("user_id"), py::arg("record_id"), py::arg("description")="")
        .FIXED_STRING_PROPERTY(PULSEvlr, "user_id", user_id)
        .def_readwrite("record_id", &PULSEvlr::record_id)
        .def_readwrite("reserved", &PULSEvlr::reserved)
        .def_readwrite("record_length_after_header", &PULSEvlr::record_length_after_header)
        .FIXED_STRING_PROPERTY(PULSEvlr, "description", description)
        .def_property("data", [](const PULSEvlr& self) {
            return py::bytes(reinterpret_cast<const char*>(self.data), self.record_length_after_header);
        }, [](PULSEvlr& self, py::bytes data) {
            std::string bytes = data;
            if (self.data) std::free(self.data);
            self.record_length_after_header = bytes.size();
            self.data = static_cast<U8*>(std::malloc(bytes.size()));
            std::memcpy(self.data, bytes.data(), bytes.size());
        });

    py::class_<PULSEavlr>(m, "PulseAVLR")
        .def(py::init<>())
        .FIXED_STRING_PROPERTY(PULSEavlr, "user_id", user_id)
        .def_readwrite("record_id", &PULSEavlr::record_id)
        .def_readwrite("reserved", &PULSEavlr::reserved)
        .def_readwrite("record_length_before_footer", &PULSEavlr::record_length_before_footer)
        .FIXED_STRING_PROPERTY(PULSEavlr, "description", description);

    py::class_<PULSEscanner>(m, "PulseScanner")
        .def(py::init<>())
        .def_readwrite("size", &PULSEscanner::size)
        .def_readwrite("reserved", &PULSEscanner::reserved)
        .FIXED_STRING_PROPERTY(PULSEscanner, "instrument", instrument)
        .FIXED_STRING_PROPERTY(PULSEscanner, "serial", serial)
        .def_readwrite("wave_length", &PULSEscanner::wave_length)
        .def_readwrite("outgoing_pulse_width", &PULSEscanner::outgoing_pulse_width)
        .def_readwrite("scan_pattern", &PULSEscanner::scan_pattern)
        .def_readwrite("number_of_mirror_facets", &PULSEscanner::number_of_mirror_facets)
        .def_readwrite("scan_frequency", &PULSEscanner::scan_frequency)
        .def_readwrite("scan_angle_min", &PULSEscanner::scan_angle_min)
        .def_readwrite("scan_angle_max", &PULSEscanner::scan_angle_max)
        .def_readwrite("pulse_frequency", &PULSEscanner::pulse_frequency)
        .def_readwrite("beam_diameter_at_exit_aperture", &PULSEscanner::beam_diameter_at_exit_aperture)
        .def_readwrite("beam_divergence", &PULSEscanner::beam_divergence)
        .def_readwrite("minimal_range", &PULSEscanner::minimal_range)
        .def_readwrite("maximal_range", &PULSEscanner::maximal_range)
        .FIXED_STRING_PROPERTY(PULSEscanner, "description", description)
        .def("size_of_attributes", &PULSEscanner::size_of_attributes);

    py::class_<PULSElookupTable>(m, "PulseLookupTable")
        .def(py::init<>())
        .def_readwrite("size", &PULSElookupTable::size)
        .def_readwrite("reserved", &PULSElookupTable::reserved)
        .def_readwrite("number_entries", &PULSElookupTable::number_entries)
        .def_readwrite("unit_of_measurement", &PULSElookupTable::unit_of_measurement)
        .def_readwrite("data_type", &PULSElookupTable::data_type)
        .def_readwrite("options", &PULSElookupTable::options)
        .def_readwrite("compression", &PULSElookupTable::compression)
        .FIXED_STRING_PROPERTY(PULSElookupTable, "description", description)
        .def("size_of_attributes", &PULSElookupTable::size_of_attributes);

    py::class_<PULSEtable>(m, "PulseTable")
        .def(py::init<>())
        .def_readwrite("size", &PULSEtable::size)
        .def_readwrite("reserved", &PULSEtable::reserved)
        .def_readwrite("number_tables", &PULSEtable::number_tables)
        .FIXED_STRING_PROPERTY(PULSEtable, "description", description)
        .def("size_of_attributes", &PULSEtable::size_of_attributes);

    py::class_<PULSEgeokeys>(m, "PulseGeoKeys")
        .def(py::init<>())
        .def_readwrite("key_directory_version", &PULSEgeokeys::key_directory_version)
        .def_readwrite("key_revision", &PULSEgeokeys::key_revision)
        .def_readwrite("minor_revision", &PULSEgeokeys::minor_revision)
        .def_readwrite("number_of_keys", &PULSEgeokeys::number_of_keys);

    py::class_<PULSEkeyentry>(m, "PulseKeyEntry")
        .def(py::init<>())
        .def_readwrite("key_id", &PULSEkeyentry::key_id)
        .def_readwrite("tiff_tag_location", &PULSEkeyentry::tiff_tag_location)
        .def_readwrite("count", &PULSEkeyentry::count)
        .def_readwrite("value_offset", &PULSEkeyentry::value_offset);

    py::class_<PULSEzip>(m, "PulseZip")
        .def(py::init<>())
        .def_readwrite("compressor", &PULSEzip::compressor)
        .def_readwrite("coder", &PULSEzip::coder)
        .def_readwrite("version_major", &PULSEzip::version_major)
        .def_readwrite("version_minor", &PULSEzip::version_minor)
        .def_readwrite("version_revision", &PULSEzip::version_revision)
        .def_readwrite("options", &PULSEzip::options)
        .def_readwrite("chunk_size", &PULSEzip::chunk_size)
        .def_readonly("num_items", &PULSEzip::num_items)
        .def("check_compressor", &PULSEzip::check_compressor)
        .def("check_coder", &PULSEzip::check_coder)
        .def("check", &PULSEzip::check)
        .def("get_payload", &PULSEzip::get_payload)
        .def("get_error", &PULSEzip::get_error)
        .def("set_chunk_size", &PULSEzip::set_chunk_size)
        .def("request_version", &PULSEzip::request_version)
        .def("setup", static_cast<BOOL (PULSEzip::*)(const U32, const U32, const U32, const U32)>(&PULSEzip::setup),
             py::arg("format"), py::arg("attributes"), py::arg("size"), py::arg("compression")=PULSEZIP_COMPRESSOR_DEFAULT)
        .def("setup_items", [](PULSEzip& self, const std::vector<PULSEitem>& items, U32 compression) {
            return self.setup(static_cast<U16>(items.size()), items.data(), compression);
        }, py::arg("items"), py::arg("compression"))
        .def("is_standard", [](PULSEzip& self) {
            U32 format = 0, attributes = 0, size = 0;
            BOOL ok = self.is_standard(&format, &attributes, &size);
            return py::make_tuple(ok, format, attributes, size);
        })
        .def("items", [](const PULSEzip& self) {
            return std::vector<PULSEitem>(self.items, self.items + self.num_items);
        });

    py::class_<PULSEpulse>(m, "Pulse")
        .def(py::init<>())
        .def_readwrite("T", &PULSEpulse::T)
        .def_readwrite("offset", &PULSEpulse::offset)
        .def_readwrite("anchor_X", &PULSEpulse::anchor_X)
        .def_readwrite("anchor_Y", &PULSEpulse::anchor_Y)
        .def_readwrite("anchor_Z", &PULSEpulse::anchor_Z)
        .def_readwrite("target_X", &PULSEpulse::target_X)
        .def_readwrite("target_Y", &PULSEpulse::target_Y)
        .def_readwrite("target_Z", &PULSEpulse::target_Z)
        .def_readwrite("first_returning_sample", &PULSEpulse::first_returning_sample)
        .def_readwrite("last_returning_sample", &PULSEpulse::last_returning_sample)
        .def_property("descriptor_index", [](const PULSEpulse& p) { return p.descriptor_index; },
                      [](PULSEpulse& p, U16 value) { p.descriptor_index = value; })
        .def_property("reserved", [](const PULSEpulse& p) { return p.reserved; },
                      [](PULSEpulse& p, U16 value) { p.reserved = value; })
        .def_property("edge_of_scan_line", [](const PULSEpulse& p) { return p.edge_of_scan_line; },
                      [](PULSEpulse& p, U16 value) { p.edge_of_scan_line = value; })
        .def_property("scan_direction", [](const PULSEpulse& p) { return p.scan_direction; },
                      [](PULSEpulse& p, U16 value) { p.scan_direction = value; })
        .def_property("mirror_facet", [](const PULSEpulse& p) { return p.mirror_facet; },
                      [](PULSEpulse& p, U16 value) { p.mirror_facet = value; })
        .def_readwrite("intensity", &PULSEpulse::intensity)
        .def_readwrite("classification", &PULSEpulse::classification)
        .def_readwrite("pulse_source_ID", &PULSEpulse::pulse_source_ID)
        .def_readwrite("has_pulse_source_ID", &PULSEpulse::has_pulse_source_ID)
        .def_readonly("total_pulse_size", &PULSEpulse::total_pulse_size)
        .def_readonly("num_items", &PULSEpulse::num_items)
        .def("init", [](PULSEpulse& self, const PULSEheader& header) { return self.init(&header); })
        .def("inside_rectangle", &PULSEpulse::inside_rectangle)
        .def("inside_tile", &PULSEpulse::inside_tile)
        .def("inside_circle", &PULSEpulse::inside_circle)
        .def("inside_box", &PULSEpulse::inside_box)
        .def("zero", &PULSEpulse::zero)
        .def("clean", &PULSEpulse::clean)
        .def("set_T", static_cast<void (PULSEpulse::*)(const I64)>(&PULSEpulse::set_T))
        .def("set_t", static_cast<void (PULSEpulse::*)(const F64)>(&PULSEpulse::set_T))
        .def("get_T", &PULSEpulse::get_T)
        .def("get_t", &PULSEpulse::get_t)
        .def("set_anchor_x", &PULSEpulse::set_anchor_x)
        .def("set_anchor_y", &PULSEpulse::set_anchor_y)
        .def("set_anchor_z", &PULSEpulse::set_anchor_z)
        .def("set_target_x", &PULSEpulse::set_target_x)
        .def("set_target_y", &PULSEpulse::set_target_y)
        .def("set_target_z", &PULSEpulse::set_target_z)
        .def("set_anchor_and_target", [](PULSEpulse& self, const std::vector<F64>& anchor, const std::vector<F64>& target) {
            if (anchor.size() != 3 || target.size() != 3) {
                throw std::invalid_argument("anchor and target must contain exactly 3 values");
            }
            self.set_anchor_and_target(anchor.data(), target.data());
        })
        .def("compute_anchor", &PULSEpulse::compute_anchor)
        .def("compute_target", &PULSEpulse::compute_target)
        .def("compute_anchor_and_target", &PULSEpulse::compute_anchor_and_target)
        .def("compute_anchor_and_target_and_dir", &PULSEpulse::compute_anchor_and_target_and_dir)
        .def("get_anchor", [](const PULSEpulse& self) {
            F64 anchor[3];
            self.get_anchor(anchor);
            return std::vector<F64>(anchor, anchor + 3);
        })
        .def("get_target", [](const PULSEpulse& self) {
            F64 target[3];
            self.get_target(target);
            return std::vector<F64>(target, target + 3);
        })
        .def("get_anchor_x", &PULSEpulse::get_anchor_x)
        .def("get_anchor_y", &PULSEpulse::get_anchor_y)
        .def("get_anchor_z", &PULSEpulse::get_anchor_z)
        .def("get_target_x", &PULSEpulse::get_target_x)
        .def("get_target_y", &PULSEpulse::get_target_y)
        .def("get_target_z", &PULSEpulse::get_target_z)
        .def("get_dir_x", &PULSEpulse::get_dir_x)
        .def("get_dir_y", &PULSEpulse::get_dir_y)
        .def("get_dir_z", &PULSEpulse::get_dir_z)
        .def("compute_first", &PULSEpulse::compute_first)
        .def("compute_last", &PULSEpulse::compute_last)
        .def("compute_first_and_last", &PULSEpulse::compute_first_and_last)
        .def("get_first_x", &PULSEpulse::get_first_x)
        .def("get_first_y", &PULSEpulse::get_first_y)
        .def("get_first_z", &PULSEpulse::get_first_z)
        .def("get_last_x", &PULSEpulse::get_last_x)
        .def("get_last_y", &PULSEpulse::get_last_y)
        .def("get_last_z", &PULSEpulse::get_last_z);

    py::class_<PULSEheader, PULSEquantizer, PULSEattributer>(m, "PulseHeader")
        .def(py::init<>())
        .FIXED_STRING_PROPERTY(PULSEheader, "file_signature", file_signature)
        .def_readwrite("global_parameters", &PULSEheader::global_parameters)
        .def_readwrite("file_source_ID", &PULSEheader::file_source_ID)
        .def_readwrite("project_ID_GUID_data_1", &PULSEheader::project_ID_GUID_data_1)
        .def_readwrite("project_ID_GUID_data_2", &PULSEheader::project_ID_GUID_data_2)
        .def_readwrite("project_ID_GUID_data_3", &PULSEheader::project_ID_GUID_data_3)
        .def_property("project_ID_GUID_data_4",
                      [](const PULSEheader& self) { return fixed_bytes(self.project_ID_GUID_data_4); },
                      [](PULSEheader& self, const std::vector<U8>& value) { set_fixed_bytes(self.project_ID_GUID_data_4, value); })
        .FIXED_STRING_PROPERTY(PULSEheader, "system_identifier", system_identifier)
        .FIXED_STRING_PROPERTY(PULSEheader, "generating_software", generating_software)
        .def_readwrite("file_creation_day", &PULSEheader::file_creation_day)
        .def_readwrite("file_creation_year", &PULSEheader::file_creation_year)
        .def_readwrite("version_major", &PULSEheader::version_major)
        .def_readwrite("version_minor", &PULSEheader::version_minor)
        .def_readwrite("header_size", &PULSEheader::header_size)
        .def_readwrite("offset_to_pulse_data", &PULSEheader::offset_to_pulse_data)
        .def_readwrite("number_of_pulses", &PULSEheader::number_of_pulses)
        .def_readwrite("pulse_format", &PULSEheader::pulse_format)
        .def_readwrite("pulse_attributes", &PULSEheader::pulse_attributes)
        .def_readwrite("pulse_size", &PULSEheader::pulse_size)
        .def_readwrite("pulse_compression", &PULSEheader::pulse_compression)
        .def_readwrite("reserved", &PULSEheader::reserved)
        .def_readwrite("number_of_variable_length_records", &PULSEheader::number_of_variable_length_records)
        .def_readwrite("number_of_appended_variable_length_records", &PULSEheader::number_of_appended_variable_length_records)
        .def_readwrite("min_T", &PULSEheader::min_T)
        .def_readwrite("max_T", &PULSEheader::max_T)
        .def_readwrite("min_x", &PULSEheader::min_x)
        .def_readwrite("max_x", &PULSEheader::max_x)
        .def_readwrite("min_y", &PULSEheader::min_y)
        .def_readwrite("max_y", &PULSEheader::max_y)
        .def_readwrite("min_z", &PULSEheader::min_z)
        .def_readwrite("max_z", &PULSEheader::max_z)
        .def_readonly("user_data_in_header_size", &PULSEheader::user_data_in_header_size)
        .def_readonly("user_data_after_header_size", &PULSEheader::user_data_after_header_size)
        .def("set_bounding_box", &PULSEheader::set_bounding_box,
             py::arg("min_x"), py::arg("min_y"), py::arg("min_z"),
             py::arg("max_x"), py::arg("max_y"), py::arg("max_z"),
             py::arg("auto_scale")=true, py::arg("auto_offset")=true)
        .def("clean_header", &PULSEheader::clean_header)
        .def("clean_user_data_in_header", &PULSEheader::clean_user_data_in_header)
        .def("clean_vlrs", &PULSEheader::clean_vlrs)
        .def("clean_user_data_after_header", &PULSEheader::clean_user_data_after_header)
        .def("clean", &PULSEheader::clean)
        .def("check", &PULSEheader::check)
        .def("add_vlr", [](PULSEheader& self, const std::string& user_id, U32 record_id, py::bytes data) {
            std::string bytes = data;
            return self.add_vlr(user_id.c_str(), record_id, bytes.size(), reinterpret_cast<const U8*>(bytes.data()));
        })
        .def("remove_vlr", static_cast<BOOL (PULSEheader::*)(U32)>(&PULSEheader::remove_vlr))
        .def("remove_vlr_by_id", [](PULSEheader& self, const std::string& user_id, U32 record_id) {
            return self.remove_vlr(user_id.c_str(), record_id);
        })
        .def("set_geodouble_params", [](PULSEheader& self, const std::vector<F64>& values) {
            return self.set_geodouble_params(values.size(), values.data());
        })
        .def("set_geoascii_params", [](PULSEheader& self, const std::string& value) {
            return self.set_geoascii_params(value.size(), value.c_str());
        })
        .def("set_geokey_entries", [](PULSEheader& self, const std::vector<PULSEkeyentry>& entries) {
            return self.set_geokey_entries(entries.size(), entries.data());
        })
        .def("del_geokey_entries", &PULSEheader::del_geokey_entries)
        .def("del_geodouble_params", &PULSEheader::del_geodouble_params)
        .def("del_geoascii_params", &PULSEheader::del_geoascii_params)
        .def("add_scanner", [](PULSEheader& self, const PULSEscanner& scanner, U32 scanner_index, bool add_to_vlrs) {
            return self.add_scanner(&scanner, scanner_index, add_to_vlrs);
        }, py::arg("scanner"), py::arg("scanner_index"), py::arg("add_to_vlrs")=true)
        .def("get_scanner", [](const PULSEheader& self, U32 scanner_index) {
            PULSEscanner scanner;
            if (!self.get_scanner(&scanner, scanner_index)) throw std::out_of_range("scanner index not found");
            return scanner;
        })
        .def("find_descriptor", [](PULSEheader& self, const PULSEcomposition& composition,
                                   const std::vector<PULSEsampling>& samplings) {
            return self.find_descriptor(&composition, samplings.data());
        })
        .def("get_descriptor", &PULSEheader::get_descriptor, py::return_value_policy::reference_internal)
        .def("get_descriptor_composition", [](const PULSEheader& self, U32 descriptor_index) {
            PULSEcomposition composition;
            if (!self.get_descriptor_composition(&composition, descriptor_index)) throw std::out_of_range("descriptor index not found");
            return composition;
        })
        .def("get_descriptor_samplings", [](const PULSEheader& self, U32 descriptor_index) {
            PULSEcomposition composition;
            if (!self.get_descriptor_composition(&composition, descriptor_index)) throw std::out_of_range("descriptor index not found");
            std::vector<PULSEsampling> samplings(composition.number_of_samplings);
            if (!self.get_descriptor_samplings(samplings.data(), descriptor_index)) throw std::out_of_range("descriptor index not found");
            return samplings;
        })
        .def("add_descriptor", [](PULSEheader& self, const PULSEcomposition& composition,
                                  const std::vector<PULSEsampling>& samplings, U32 descriptor_index,
                                  bool add_to_vlrs) {
            if (samplings.size() < composition.number_of_samplings) throw std::invalid_argument("not enough sampling records");
            return self.add_descriptor(&composition, samplings.data(), descriptor_index, add_to_vlrs);
        }, py::arg("composition"), py::arg("samplings"), py::arg("descriptor_index"), py::arg("add_to_vlrs")=true)
        .def("add_descriptor_assign_index", [](PULSEheader& self, const PULSEcomposition& composition,
                                               const std::vector<PULSEsampling>& samplings, bool add_to_vlrs) {
            if (samplings.size() < composition.number_of_samplings) throw std::invalid_argument("not enough sampling records");
            return self.add_descriptor_assign_index(&composition, samplings.data(), add_to_vlrs);
        }, py::arg("composition"), py::arg("samplings"), py::arg("add_to_vlrs")=true)
        .def("update_extra_bytes", &PULSEheader::update_extra_bytes);

    py::class_<PULSEinventory>(m, "PulseInventory")
        .def(py::init<>())
        .def_readonly("number_of_pulses", &PULSEinventory::number_of_pulses)
        .def_readonly("min_T", &PULSEinventory::min_T)
        .def_readonly("max_T", &PULSEinventory::max_T)
        .def_readonly("min_x", &PULSEinventory::min_x)
        .def_readonly("max_x", &PULSEinventory::max_x)
        .def_readonly("min_y", &PULSEinventory::min_y)
        .def_readonly("max_y", &PULSEinventory::max_y)
        .def_readonly("min_z", &PULSEinventory::min_z)
        .def_readonly("max_z", &PULSEinventory::max_z)
        .def("active", &PULSEinventory::active)
        .def("add", [](PULSEinventory& self, const PULSEpulse& pulse, bool only_count_pulses) {
            return self.add(&pulse, only_count_pulses);
        }, py::arg("pulse"), py::arg("only_count_pulses")=false);

    py::class_<PULSEsummary>(m, "PulseSummary")
        .def(py::init<>())
        .def_readonly("number_of_pulses", &PULSEsummary::number_of_pulses)
        .def_readonly("min", &PULSEsummary::min)
        .def_readonly("max", &PULSEsummary::max)
        .def_readonly("min_x", &PULSEsummary::min_x)
        .def_readonly("max_x", &PULSEsummary::max_x)
        .def_readonly("min_y", &PULSEsummary::min_y)
        .def_readonly("max_y", &PULSEsummary::max_y)
        .def_readonly("min_z", &PULSEsummary::min_z)
        .def_readonly("max_z", &PULSEsummary::max_z)
        .def("active", &PULSEsummary::active)
        .def("add", [](PULSEsummary& self, const PULSEpulse& pulse) {
            return self.add(&pulse);
        });

    py::class_<PULSEbin>(m, "PulseBin")
        .def(py::init<F32>())
        .def("add_int", static_cast<void (PULSEbin::*)(I32)>(&PULSEbin::add))
        .def("add_int64", static_cast<void (PULSEbin::*)(I64)>(&PULSEbin::add))
        .def("add_float", static_cast<void (PULSEbin::*)(F64)>(&PULSEbin::add))
        .def("add_value", static_cast<void (PULSEbin::*)(I32, I32)>(&PULSEbin::add))
        .def("snapshot", &pulse_bin_snapshot);

    py::class_<PULSEhistogram>(m, "PulseHistogram")
        .def(py::init<>())
        .def("active", &PULSEhistogram::active)
        .def("parse", [](PULSEhistogram& self, const std::vector<std::string>& arguments) {
            std::vector<std::string> mutable_args = arguments;
            std::vector<char*> argv = argv_from_args(mutable_args);
            return self.parse(static_cast<int>(argv.size()), argv.data());
        })
        .def("histo", &PULSEhistogram::histo)
        .def("histo_avg", &PULSEhistogram::histo_avg)
        .def("add", [](PULSEhistogram& self, const PULSEpulse& pulse) {
            self.add(&pulse);
        })
        .def("snapshot", &pulse_histogram_snapshot);

    py::class_<PULSEoccupancyGrid>(m, "PulseOccupancyGrid")
        .def(py::init<F32>())
        .def_readonly("min_x", &PULSEoccupancyGrid::min_x)
        .def_readonly("min_y", &PULSEoccupancyGrid::min_y)
        .def_readonly("max_x", &PULSEoccupancyGrid::max_x)
        .def_readonly("max_y", &PULSEoccupancyGrid::max_y)
        .def("reset", &PULSEoccupancyGrid::reset)
        .def("add", static_cast<BOOL (PULSEoccupancyGrid::*)(I32, I32)>(&PULSEoccupancyGrid::add))
        .def("add_pulse", [](PULSEoccupancyGrid& self, const PULSEpulse& pulse) { return self.add(&pulse); })
        .def("occupied", static_cast<BOOL (PULSEoccupancyGrid::*)(I32, I32) const>(&PULSEoccupancyGrid::occupied))
        .def("occupied_pulse", [](const PULSEoccupancyGrid& self, const PULSEpulse& pulse) { return self.occupied(&pulse); })
        .def("active", &PULSEoccupancyGrid::active)
        .def("get_num_occupied", &PULSEoccupancyGrid::get_num_occupied)
        .def("write_asc_grid", &PULSEoccupancyGrid::write_asc_grid);

    py::class_<PULSEfilter>(m, "PulseFilter")
        .def(py::init<>())
        .def("clean", &PULSEfilter::clean)
        .def("active", &PULSEfilter::active)
        .def("reset", &PULSEfilter::reset)
        .def("parse", [](PULSEfilter& self, const std::vector<std::string>& arguments) {
            std::vector<std::string> mutable_args = arguments;
            std::vector<char*> argv = argv_from_args(mutable_args);
            return self.parse(static_cast<int>(argv.size()), argv.data());
        })
        .def("filter", [](PULSEfilter& self, const PULSEpulse& pulse) { return self.filter(&pulse); })
        .def("unparse", [](const PULSEfilter& self) {
            std::vector<char> buffer(4096);
            self.unparse(buffer.data());
            return std::string(buffer.data());
        })
        .def("addKeepCircle", &PULSEfilter::addKeepCircle)
        .def("addKeepBox", &PULSEfilter::addKeepBox);

    py::class_<PULSEtransform>(m, "PulseTransform")
        .def(py::init<>())
        .def("clean", &PULSEtransform::clean)
        .def("active", &PULSEtransform::active)
        .def("parse", [](PULSEtransform& self, const std::vector<std::string>& arguments) {
            std::vector<std::string> mutable_args = arguments;
            std::vector<char*> argv = argv_from_args(mutable_args);
            return self.parse(static_cast<int>(argv.size()), argv.data());
        })
        .def("transform", [](const PULSEtransform& self, PULSEpulse& pulse) {
            self.transform(&pulse);
        })
        .def("unparse", [](const PULSEtransform& self) {
            std::vector<char> buffer(4096);
            self.unparse(buffer.data());
            return std::string(buffer.data());
        });

    py::class_<PULSEindex>(m, "PulseIndex")
        .def(py::init<>())
        .def("read", &PULSEindex::read)
        .def("intersect_rectangle", &PULSEindex::intersect_rectangle)
        .def("intersect_tile", &PULSEindex::intersect_tile)
        .def("intersect_circle", &PULSEindex::intersect_circle);

    py::class_<PulseReaderWrapper>(m, "PulseReader")
        .def(py::init<>())
        .def("open", &PulseReaderWrapper::open)
        .def("open_with_args", &PulseReaderWrapper::open_with_args)
        .def("close", &PulseReaderWrapper::close)
        .def("read_pulse", &PulseReaderWrapper::read_pulse)
        .def("read_waves", &PulseReaderWrapper::read_waves)
        .def("seek", &PulseReaderWrapper::seek)
        .def("get_format", &PulseReaderWrapper::get_format)
        .def("get_n_pulses", &PulseReaderWrapper::get_n_pulses)
        .def("get_header", &PulseReaderWrapper::get_header, py::return_value_policy::reference_internal)
        .def("get_pulse", &PulseReaderWrapper::get_pulse, py::return_value_policy::reference_internal);

    py::class_<PulseWriterWrapper>(m, "PulseWriter")
        .def(py::init<>())
        .def("open", &PulseWriterWrapper::open)
        .def("open_with_args", &PulseWriterWrapper::open_with_args)
        .def("close", &PulseWriterWrapper::close, py::arg("update_npulses")=true)
        .def("write_pulse", &PulseWriterWrapper::write_pulse)
        .def("update_header", &PulseWriterWrapper::update_header,
             py::arg("header"), py::arg("use_inventory")=true, py::arg("update_extra_bytes")=false)
        .def("get_current_offset", &PulseWriterWrapper::get_current_offset);

    m.def("version", []() {
        return std::string("PulseWaves ") +
               std::to_string(PULSEWAVES_VERSION_MAJOR) + "." +
               std::to_string(PULSEWAVES_VERSION_MINOR) + " r" +
               std::to_string(PULSEWAVES_REVISION);
    });
}

#undef FIXED_STRING_PROPERTY
