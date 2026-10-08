"""Native PulseWaves bindings.

The runtime module is implemented in C++ with pybind11. This stub documents the
Python-facing signatures for the wrapped C++ classes and functions.
"""

from __future__ import annotations

from enum import Enum
from typing import overload

PULSEWAVES_VERSION_MAJOR: int
PULSEWAVES_VERSION_MINOR: int
PULSEWAVES_REVISION: int
PULSEWAVES_BUILD_DATE: int
PULSEWAVES_UNDEFINED: int
PULSEWAVES_OUTGOING: int
PULSEWAVES_RETURNING: int
PULSEWAVES_UNCOMPRESSED: int
PULSEWAVES_FORMAT_DEFAULT: int
PULSEWAVES_FORMAT_PLS: int
PULSEWAVES_FORMAT_PLZ: int
PULSEWAVES_FORMAT_LAS: int
PULSEWAVES_FORMAT_LAZ: int
PULSEWAVES_FORMAT_LGW: int
PULSEWAVES_FORMAT_GCW: int
PULSEWAVES_FORMAT_SDF: int
PULSEWAVES_FORMAT_CSD: int
PULSEWAVES_FORMAT_DAT: int
PULSEWAVES_FORMAT_TXT: int
PULSE_EXTRA_ATTRIBUTE_U8: int
PULSE_EXTRA_ATTRIBUTE_I8: int
PULSE_EXTRA_ATTRIBUTE_U16: int
PULSE_EXTRA_ATTRIBUTE_I16: int
PULSE_EXTRA_ATTRIBUTE_U32: int
PULSE_EXTRA_ATTRIBUTE_I32: int
PULSE_EXTRA_ATTRIBUTE_U64: int
PULSE_EXTRA_ATTRIBUTE_I64: int
PULSE_EXTRA_ATTRIBUTE_F32: int
PULSE_EXTRA_ATTRIBUTE_F64: int
PULSEZIP_COMPRESSOR_NONE: int
PULSEZIP_COMPRESSOR_DEFAULT: int
PULSEZIP_CODER_ARITHMETIC: int
PULSEZIP_CHUNK_SIZE_DEFAULT: int


class PulseItemType(Enum):
    """Pulse item kinds supported by PulseWaves."""

    EXTRABYTES = ...
    PULSE0 = ...
    PULSESOURCEID16 = ...
    PULSESOURCEID32 = ...


class PulseQuantizer:
    """Quantize between floating point times/coordinates and raw integers."""

    t_scale_factor: float
    t_offset: float
    x_scale_factor: float
    y_scale_factor: float
    z_scale_factor: float
    x_offset: float
    y_offset: float
    z_offset: float

    def __init__(self) -> None: ...

    def get_T(self, t: float) -> int:
        """Return the raw integer timestamp for scaled GPS time ``t``."""
        ...

    def get_T_msec(self, T: int) -> int:
        """Return raw timestamp ``T`` converted to milliseconds."""
        ...

    def get_T_usec(self, T: int) -> int:
        """Return raw timestamp ``T`` converted to microseconds."""
        ...

    def get_T_nsec(self, T: int) -> int:
        """Return raw timestamp ``T`` converted to nanoseconds."""
        ...

    def get_T_psec(self, T: int) -> int:
        """Return raw timestamp ``T`` converted to picoseconds."""
        ...

    def get_t(self, T: int) -> float:
        """Return the scaled GPS time for raw timestamp ``T``."""
        ...

    def get_t_msec(self, T: int) -> float:
        """Return scaled timestamp ``T`` in milliseconds."""
        ...

    def get_t_usec(self, T: int) -> float:
        """Return scaled timestamp ``T`` in microseconds."""
        ...

    def get_t_nsec(self, T: int) -> float:
        """Return scaled timestamp ``T`` in nanoseconds."""
        ...

    def get_t_psec(self, T: int) -> float:
        """Return scaled timestamp ``T`` in picoseconds."""
        ...

    def get_x(self, X: int) -> float:
        """Return the scaled x coordinate for raw coordinate ``X``."""
        ...

    def get_y(self, Y: int) -> float:
        """Return the scaled y coordinate for raw coordinate ``Y``."""
        ...

    def get_z(self, Z: int) -> float:
        """Return the scaled z coordinate for raw coordinate ``Z``."""
        ...

    def get_X(self, x: float) -> int:
        """Return the raw integer X coordinate for scaled coordinate ``x``."""
        ...

    def get_Y(self, y: float) -> int:
        """Return the raw integer Y coordinate for scaled coordinate ``y``."""
        ...

    def get_Z(self, z: float) -> int:
        """Return the raw integer Z coordinate for scaled coordinate ``z``."""
        ...


class PulseItem:
    """Description of one serialized item in a pulse record."""

    type: PulseItemType
    size: int
    version: int

    def __init__(self) -> None: ...

    def is_type(self, type: PulseItemType) -> bool:
        """Return whether this item has ``type``."""
        ...

    def get_name(self) -> str:
        """Return PulseWaves' display name for this item type."""
        ...


class PulseAttribute:
    """Extra attribute descriptor attached to a pulse record."""

    data_type: int
    options: int
    name: str
    offset_to_first_byte: int
    description: str

    @staticmethod
    def from_size(size: int) -> PulseAttribute:
        """Create a byte-sized extra attribute with ``size`` bytes."""
        ...

    @staticmethod
    def from_type(
        type: int, name: str, description: str = "", dim: int = 1
    ) -> PulseAttribute:
        """Create an extra attribute with a PulseWaves data ``type``."""
        ...

    def set_scale(self, scale: float, dim: int = 0) -> None:
        """Set the numeric scale for dimension ``dim``."""
        ...

    def set_offset(self, offset: float, dim: int = 0) -> None:
        """Set the numeric offset for dimension ``dim``."""
        ...

    def has_no_data(self) -> bool:
        """Return whether the no-data option is set."""
        ...

    def has_min(self) -> bool:
        """Return whether a minimum value is recorded."""
        ...

    def has_max(self) -> bool:
        """Return whether a maximum value is recorded."""
        ...

    def has_scale(self) -> bool:
        """Return whether a scale is recorded."""
        ...

    def has_offset(self) -> bool:
        """Return whether an offset is recorded."""
        ...

    def get_size(self) -> int:
        """Return the attribute size in bytes."""
        ...


class PulseAttributer:
    """Container that manages extra pulse attributes."""

    number_extra_attributes: int

    def __init__(self) -> None: ...

    def clean_extra_attributes(self) -> None:
        """Remove all extra attribute descriptors."""
        ...

    def add_extra_attribute(self, extra_attribute: PulseAttribute) -> int:
        """Append ``extra_attribute`` and return its index, or -1 on failure."""
        ...

    def get_total_extra_attributes_size(self) -> int:
        """Return the total size of all extra attributes in bytes."""
        ...

    def get_extra_attribute_index(self, name: str) -> int:
        """Return the index of the extra attribute named ``name``."""
        ...

    def get_extra_attribute_array_offset_by_name(self, name: str) -> int:
        """Return the byte offset of the extra attribute named ``name``."""
        ...

    def get_extra_attribute_array_offset(self, index: int) -> int:
        """Return the byte offset of the extra attribute at ``index``."""
        ...

    @overload
    def remove_extra_attribute(self, index: int) -> bool:
        """Remove the extra attribute at ``index``."""
        ...

    @overload
    def remove_extra_attribute(self, name: str) -> bool:
        """Remove the extra attribute named ``name``."""
        ...


class PulseSampling:
    """Waveform sampling descriptor."""

    size: int
    reserved: int
    type: int
    channel: int
    unused: int
    bits_for_duration_from_anchor: int
    scale_for_duration_from_anchor: int
    offset_for_duration_from_anchor: int
    bits_for_number_of_segments: int
    bits_for_number_of_samples: int
    number_of_segments: int
    number_of_samples: int
    bits_per_sample: int
    lookup_table_index: int
    sample_units: float
    compression: int
    description: str

    def __init__(self) -> None: ...

    def size_of_attributes(self) -> int:
        """Return the serialized size of this sampling descriptor."""
        ...

    def is_equal(self, other: PulseSampling) -> bool:
        """Return whether ``other`` describes the same sampling layout."""
        ...


class PulseComposition:
    """Composition record for a pulse descriptor."""

    size: int
    reserved: int
    optical_center_to_anchor_point: int
    number_of_extra_waves_bytes: int
    number_of_samplings: int
    scanner_index: int
    sample_units: float
    compression: int
    description: str

    def __init__(self) -> None: ...

    def size_of_attributes(self) -> int:
        """Return the serialized size of this composition descriptor."""
        ...

    def is_equal(self, other: PulseComposition) -> bool:
        """Return whether ``other`` describes the same pulse composition."""
        ...


class PulseDescriptor:
    """Combined pulse composition and sampling descriptor."""

    def __init__(self) -> None: ...

    def is_equal_descriptor(self, other: PulseDescriptor) -> bool:
        """Return whether ``other`` is equivalent to this descriptor."""
        ...

    def is_equal(
        self, composition: PulseComposition, samplings: list[PulseSampling]
    ) -> bool:
        """Return whether ``composition`` and ``samplings`` match this descriptor."""
        ...


class PulseVLR:
    """Variable-length record stored in a PulseWaves header."""

    user_id: str
    record_id: int
    reserved: int
    record_length_after_header: int
    description: str
    data: bytes

    @overload
    def __init__(self) -> None: ...

    @overload
    def __init__(
        self, user_id: str, record_id: int, description: str = ""
    ) -> None: ...


class PulseAVLR:
    """Appended variable-length record stored near the end of a file."""

    user_id: str
    record_id: int
    reserved: int
    record_length_before_footer: int
    description: str

    def __init__(self) -> None: ...


class PulseScanner:
    """Scanner metadata referenced by pulse descriptors."""

    size: int
    reserved: int
    instrument: str
    serial: str
    wave_length: int
    outgoing_pulse_width: int
    scan_pattern: int
    number_of_mirror_facets: int
    scan_frequency: float
    scan_angle_min: float
    scan_angle_max: float
    pulse_frequency: float
    beam_diameter_at_exit_aperture: float
    beam_divergence: float
    minimal_range: float
    maximal_range: float
    description: str

    def __init__(self) -> None: ...

    def size_of_attributes(self) -> int:
        """Return the serialized size of this scanner descriptor."""
        ...


class PulseLookupTable:
    """Lookup table metadata for waveform samples."""

    size: int
    reserved: int
    number_entries: int
    unit_of_measurement: int
    data_type: int
    options: int
    compression: int
    description: str

    def __init__(self) -> None: ...

    def size_of_attributes(self) -> int:
        """Return the serialized size of this lookup table descriptor."""
        ...


class PulseTable:
    """Table metadata for waveform samples."""

    size: int
    reserved: int
    number_tables: int
    description: str

    def __init__(self) -> None: ...

    def size_of_attributes(self) -> int:
        """Return the serialized size of this table descriptor."""
        ...


class PulseGeoKeys:
    """GeoTIFF key directory header."""

    key_directory_version: int
    key_revision: int
    minor_revision: int
    number_of_keys: int

    def __init__(self) -> None: ...


class PulseKeyEntry:
    """GeoTIFF key entry."""

    key_id: int
    tiff_tag_location: int
    count: int
    value_offset: int

    def __init__(self) -> None: ...


class PulseZip:
    """PulseWaves compression descriptor."""

    compressor: int
    coder: int
    version_major: int
    version_minor: int
    version_revision: int
    options: int
    chunk_size: int
    num_items: int

    def __init__(self) -> None: ...

    def check_compressor(self, compressor: int) -> bool:
        """Return whether ``compressor`` is supported."""
        ...

    def check_coder(self, coder: int) -> bool:
        """Return whether ``coder`` is supported."""
        ...

    def check(self) -> bool:
        """Validate the current compression descriptor."""
        ...

    def get_payload(self) -> int:
        """Return the serialized payload size in bytes."""
        ...

    def get_error(self) -> str:
        """Return the last validation/setup error from the descriptor."""
        ...

    def set_chunk_size(self, chunk_size: int) -> bool:
        """Set the compressor chunk size."""
        ...

    def request_version(self, requested_version: int) -> bool:
        """Request a compressor version."""
        ...

    def setup(
        self,
        format: int,
        attributes: int,
        size: int,
        compression: int = PULSEZIP_COMPRESSOR_DEFAULT,
    ) -> bool:
        """Configure compression from a standard pulse format description."""
        ...

    def setup_items(self, items: list[PulseItem], compression: int) -> bool:
        """Configure compression from explicit pulse ``items``."""
        ...

    def is_standard(self) -> tuple[bool, int, int, int]:
        """Return ``(ok, format, attributes, size)`` for the current items."""
        ...

    def items(self) -> list[PulseItem]:
        """Return this descriptor's pulse item records."""
        ...


class Pulse:
    """One PulseWaves pulse record."""

    T: int
    offset: int
    anchor_X: int
    anchor_Y: int
    anchor_Z: int
    target_X: int
    target_Y: int
    target_Z: int
    first_returning_sample: int
    last_returning_sample: int
    descriptor_index: int
    reserved: int
    edge_of_scan_line: int
    scan_direction: int
    mirror_facet: int
    intensity: int
    classification: int
    pulse_source_ID: int
    has_pulse_source_ID: int
    total_pulse_size: int
    num_items: int

    def __init__(self) -> None: ...

    def init(self, header: PulseHeader) -> bool:
        """Initialize this pulse using ``header`` metadata."""
        ...

    def inside_rectangle(
        self, r_min_x: float, r_min_y: float, r_max_x: float, r_max_y: float
    ) -> bool:
        """Return whether the pulse is inside a 2D rectangle."""
        ...

    def inside_tile(
        self, ll_x: float, ll_y: float, ur_x: float, ur_y: float
    ) -> bool:
        """Return whether the pulse is inside a tile rectangle."""
        ...

    def inside_circle(
        self, center_x: float, center_y: float, squared_radius: float
    ) -> bool:
        """Return whether the pulse is inside a circle."""
        ...

    def inside_box(
        self,
        min_x: float,
        min_y: float,
        min_z: float,
        max_x: float,
        max_y: float,
        max_z: float,
    ) -> bool:
        """Return whether the pulse is inside a 3D box."""
        ...

    def zero(self) -> None:
        """Reset pulse fields to zero values."""
        ...

    def clean(self) -> None:
        """Release owned pulse buffers and reset this pulse."""
        ...

    def set_T(self, T: int) -> None:
        """Set the raw integer timestamp."""
        ...

    def set_t(self, t: float) -> None:
        """Set the scaled timestamp using the pulse quantizer."""
        ...

    def get_T(self) -> int:
        """Return the raw integer timestamp."""
        ...

    def get_t(self) -> float:
        """Return the scaled timestamp."""
        ...

    def set_anchor_x(self, anchor_x: float) -> None:
        """Set the scaled anchor x coordinate."""
        ...

    def set_anchor_y(self, anchor_y: float) -> None:
        """Set the scaled anchor y coordinate."""
        ...

    def set_anchor_z(self, anchor_z: float) -> None:
        """Set the scaled anchor z coordinate."""
        ...

    def set_target_x(self, target_x: float) -> None:
        """Set the scaled target x coordinate."""
        ...

    def set_target_y(self, target_y: float) -> None:
        """Set the scaled target y coordinate."""
        ...

    def set_target_z(self, target_z: float) -> None:
        """Set the scaled target z coordinate."""
        ...

    def set_anchor_and_target(
        self, anchor: list[float], target: list[float]
    ) -> None:
        """Set scaled 3D anchor and target coordinates."""
        ...

    def compute_anchor(self) -> None:
        """Compute scaled anchor coordinates from raw anchor integers."""
        ...

    def compute_target(self) -> None:
        """Compute scaled target coordinates from raw target integers."""
        ...

    def compute_anchor_and_target(self) -> None:
        """Compute scaled anchor and target coordinates from raw integers."""
        ...

    def compute_anchor_and_target_and_dir(self) -> None:
        """Compute scaled anchor, target, and direction vectors."""
        ...

    def get_anchor(self) -> list[float]:
        """Return ``[x, y, z]`` for the cached scaled anchor."""
        ...

    def get_target(self) -> list[float]:
        """Return ``[x, y, z]`` for the cached scaled target."""
        ...

    def get_anchor_x(self) -> float:
        """Return the cached scaled anchor x coordinate."""
        ...

    def get_anchor_y(self) -> float:
        """Return the cached scaled anchor y coordinate."""
        ...

    def get_anchor_z(self) -> float:
        """Return the cached scaled anchor z coordinate."""
        ...

    def get_target_x(self) -> float:
        """Return the cached scaled target x coordinate."""
        ...

    def get_target_y(self) -> float:
        """Return the cached scaled target y coordinate."""
        ...

    def get_target_z(self) -> float:
        """Return the cached scaled target z coordinate."""
        ...

    def get_dir_x(self) -> float:
        """Return the cached direction x component."""
        ...

    def get_dir_y(self) -> float:
        """Return the cached direction y component."""
        ...

    def get_dir_z(self) -> float:
        """Return the cached direction z component."""
        ...

    def compute_first(self) -> None:
        """Compute the first returning sample coordinate."""
        ...

    def compute_last(self) -> None:
        """Compute the last returning sample coordinate."""
        ...

    def compute_first_and_last(self) -> None:
        """Compute the first and last returning sample coordinates."""
        ...

    def get_first_x(self) -> float:
        """Return the cached first returning sample x coordinate."""
        ...

    def get_first_y(self) -> float:
        """Return the cached first returning sample y coordinate."""
        ...

    def get_first_z(self) -> float:
        """Return the cached first returning sample z coordinate."""
        ...

    def get_last_x(self) -> float:
        """Return the cached last returning sample x coordinate."""
        ...

    def get_last_y(self) -> float:
        """Return the cached last returning sample y coordinate."""
        ...

    def get_last_z(self) -> float:
        """Return the cached last returning sample z coordinate."""
        ...


class PulseHeader(PulseQuantizer, PulseAttributer):
    """PulseWaves file header and VLR metadata."""

    file_signature: str
    global_parameters: int
    file_source_ID: int
    project_ID_GUID_data_1: int
    project_ID_GUID_data_2: int
    project_ID_GUID_data_3: int
    project_ID_GUID_data_4: list[int]
    system_identifier: str
    generating_software: str
    file_creation_day: int
    file_creation_year: int
    version_major: int
    version_minor: int
    header_size: int
    offset_to_pulse_data: int
    number_of_pulses: int
    pulse_format: int
    pulse_attributes: int
    pulse_size: int
    pulse_compression: int
    reserved: int
    number_of_variable_length_records: int
    number_of_appended_variable_length_records: int
    min_T: int
    max_T: int
    min_x: float
    max_x: float
    min_y: float
    max_y: float
    min_z: float
    max_z: float
    user_data_in_header_size: int
    user_data_after_header_size: int

    def __init__(self) -> None: ...

    def set_bounding_box(
        self,
        min_x: float,
        min_y: float,
        min_z: float,
        max_x: float,
        max_y: float,
        max_z: float,
        auto_scale: bool = True,
        auto_offset: bool = True,
    ) -> None:
        """Set header bounds and optionally derive quantizer scale/offset."""
        ...

    def clean_header(self) -> None:
        """Reset fixed-size header fields."""
        ...

    def clean_user_data_in_header(self) -> None:
        """Remove user data stored inside the header."""
        ...

    def clean_vlrs(self) -> None:
        """Remove variable-length records."""
        ...

    def clean_user_data_after_header(self) -> None:
        """Remove user data stored after the header."""
        ...

    def clean(self) -> None:
        """Reset the header and release owned metadata buffers."""
        ...

    def check(self) -> bool:
        """Validate the header."""
        ...

    def add_vlr(self, user_id: str, record_id: int, data: bytes) -> bool:
        """Append a variable-length record."""
        ...

    def remove_vlr(self, index: int) -> bool:
        """Remove a VLR by zero-based ``index``."""
        ...

    def remove_vlr_by_id(self, user_id: str, record_id: int) -> bool:
        """Remove a VLR by ``user_id`` and ``record_id``."""
        ...

    def set_geodouble_params(self, values: list[float]) -> bool:
        """Replace GeoTIFF double parameters."""
        ...

    def set_geoascii_params(self, value: str) -> bool:
        """Replace GeoTIFF ASCII parameters."""
        ...

    def set_geokey_entries(self, entries: list[PulseKeyEntry]) -> bool:
        """Replace GeoTIFF key entries."""
        ...

    def del_geokey_entries(self) -> None:
        """Delete GeoTIFF key entries."""
        ...

    def del_geodouble_params(self) -> None:
        """Delete GeoTIFF double parameters."""
        ...

    def del_geoascii_params(self) -> None:
        """Delete GeoTIFF ASCII parameters."""
        ...

    def add_scanner(
        self, scanner: PulseScanner, scanner_index: int, add_to_vlrs: bool = True
    ) -> bool:
        """Add ``scanner`` under ``scanner_index``."""
        ...

    def get_scanner(self, scanner_index: int) -> PulseScanner:
        """Return the scanner stored at ``scanner_index``."""
        ...

    def find_descriptor(
        self, composition: PulseComposition, samplings: list[PulseSampling]
    ) -> int:
        """Return the descriptor index matching ``composition`` and ``samplings``."""
        ...

    def get_descriptor(self, descriptor_index: int) -> PulseDescriptor:
        """Return a descriptor by ``descriptor_index``."""
        ...

    def get_descriptor_composition(
        self, descriptor_index: int
    ) -> PulseComposition:
        """Return the composition for ``descriptor_index``."""
        ...

    def get_descriptor_samplings(
        self, descriptor_index: int
    ) -> list[PulseSampling]:
        """Return the sampling records for ``descriptor_index``."""
        ...

    def add_descriptor(
        self,
        composition: PulseComposition,
        samplings: list[PulseSampling],
        descriptor_index: int,
        add_to_vlrs: bool = True,
    ) -> bool:
        """Add a descriptor with an explicit ``descriptor_index``."""
        ...

    def add_descriptor_assign_index(
        self,
        composition: PulseComposition,
        samplings: list[PulseSampling],
        add_to_vlrs: bool = True,
    ) -> int:
        """Add a descriptor and return the assigned descriptor index."""
        ...

    def update_extra_bytes(self) -> bool:
        """Synchronize extra-byte metadata from extra attributes."""
        ...


class PulseInventory:
    """Accumulate pulse count and bounding box metadata."""

    number_of_pulses: int
    min_T: int
    max_T: int
    min_x: float
    max_x: float
    min_y: float
    max_y: float
    min_z: float
    max_z: float

    def __init__(self) -> None: ...

    def active(self) -> bool:
        """Return whether at least one pulse has been added."""
        ...

    def add(self, pulse: Pulse, only_count_pulses: bool = False) -> bool:
        """Add ``pulse`` to the inventory."""
        ...


class PulseSummary:
    """Accumulate min/max pulse summaries."""

    number_of_pulses: int
    min: Pulse
    max: Pulse
    min_x: float
    max_x: float
    min_y: float
    max_y: float
    min_z: float
    max_z: float

    def __init__(self) -> None: ...

    def active(self) -> bool:
        """Return whether at least one pulse has been added."""
        ...

    def add(self, pulse: Pulse) -> bool:
        """Add ``pulse`` to the summary."""
        ...


class PulseBin:
    """One-dimensional histogram bin accumulator."""

    def __init__(self, step: float) -> None:
        """Create bins with width ``step``."""
        ...

    def add_int(self, item: int) -> None:
        """Add an integer sample."""
        ...

    def add_int64(self, item: int) -> None:
        """Add a 64-bit integer sample."""
        ...

    def add_float(self, item: float) -> None:
        """Add a floating-point sample."""
        ...

    def add_value(self, item: int, value: int) -> None:
        """Add ``value`` to the bin that contains ``item``."""
        ...


class PulseHistogram:
    """Collection of named PulseWaves histograms."""

    def __init__(self) -> None: ...

    def active(self) -> bool:
        """Return whether any histograms are configured."""
        ...

    def parse(self, arguments: list[str]) -> bool:
        """Configure histograms from C++ ``-histo`` argument strings."""
        ...

    def histo(self, name: str, step: float) -> bool:
        """Add a histogram for pulse field ``name`` using bin width ``step``."""
        ...

    def histo_avg(self, name: str, step: float, name_avg: str) -> bool:
        """Add a histogram of ``name_avg`` averaged by bins of ``name``."""
        ...

    def add(self, pulse: Pulse) -> None:
        """Add ``pulse`` to all configured histograms."""
        ...


class PulseOccupancyGrid:
    """Sparse 2D occupancy grid over pulse anchor locations."""

    min_x: int
    min_y: int
    max_x: int
    max_y: int

    def __init__(self, grid_spacing: float) -> None:
        """Create a grid whose cells are ``grid_spacing`` units wide."""
        ...

    def reset(self) -> None:
        """Clear occupied cells."""
        ...

    def add(self, pos_x: int, pos_y: int) -> bool:
        """Mark integer grid coordinate ``(pos_x, pos_y)`` as occupied."""
        ...

    def add_pulse(self, pulse: Pulse) -> bool:
        """Mark the cell containing ``pulse`` as occupied."""
        ...

    def occupied(self, pos_x: int, pos_y: int) -> bool:
        """Return whether integer grid coordinate ``(pos_x, pos_y)`` is occupied."""
        ...

    def occupied_pulse(self, pulse: Pulse) -> bool:
        """Return whether the cell containing ``pulse`` is occupied."""
        ...

    def active(self) -> bool:
        """Return whether the grid contains at least one occupied cell."""
        ...

    def get_num_occupied(self) -> int:
        """Return the number of occupied cells."""
        ...

    def write_asc_grid(self, file_name: str) -> bool:
        """Write the grid to an Esri ASCII grid file."""
        ...


class PulseFilter:
    """Predicate that drops pulses matching configured filter criteria."""

    def __init__(self) -> None: ...

    def clean(self) -> None:
        """Remove all filter criteria."""
        ...

    def active(self) -> bool:
        """Return whether any filter criteria are configured."""
        ...

    def reset(self) -> None:
        """Reset per-criterion counters."""
        ...

    def parse(self, arguments: list[str]) -> bool:
        """Configure filters from C++ ``-keep_*``/``-drop_*`` argument strings."""
        ...

    def filter(self, pulse: Pulse) -> bool:
        """Return ``True`` when ``pulse`` should be dropped."""
        ...

    def unparse(self) -> str:
        """Return this filter as C++ filter argument text."""
        ...

    def addKeepCircle(self, x: float, y: float, radius: float) -> None:
        """Keep pulses inside the 2D circle centered at ``(x, y)``."""
        ...

    def addKeepBox(
        self,
        min_x: float,
        min_y: float,
        min_z: float,
        max_x: float,
        max_y: float,
        max_z: float,
    ) -> None:
        """Keep pulses inside the 3D box."""
        ...


class PulseTransform:
    """Mutating pulse transform configured with C++ transform arguments."""

    def __init__(self) -> None: ...

    def clean(self) -> None:
        """Remove all transform operations."""
        ...

    def active(self) -> bool:
        """Return whether any transform operations are configured."""
        ...

    def parse(self, arguments: list[str]) -> bool:
        """Configure transforms from C++ transform argument strings."""
        ...

    def transform(self, pulse: Pulse) -> None:
        """Apply configured transforms to ``pulse`` in place."""
        ...

    def unparse(self) -> str:
        """Return this transform as C++ transform argument text."""
        ...


class PulseIndex:
    """Spatial index placeholder for PulseWaves pulse files."""

    def __init__(self) -> None: ...

    def read(self, file_name: str) -> bool:
        """Read an index from ``file_name``."""
        ...

    def intersect_rectangle(
        self, r_min_x: float, r_min_y: float, r_max_x: float, r_max_y: float
    ) -> bool:
        """Intersect the index with a 2D rectangle."""
        ...

    def intersect_tile(self, ll_x: float, ll_y: float, size: float) -> bool:
        """Intersect the index with a square tile."""
        ...

    def intersect_circle(
        self, center_x: float, center_y: float, radius: float
    ) -> bool:
        """Intersect the index with a 2D circle."""
        ...


class PulseReader:
    """Open PulseWaves-compatible files and iterate over pulses."""

    def __init__(self) -> None: ...

    def open(self, filename: str) -> bool:
        """Open one pulse file by path."""
        ...

    def open_with_args(self, arguments: list[str]) -> bool:
        """Open a reader configured by ``PULSEreadOpener`` argument strings."""
        ...

    def close(self) -> None:
        """Close the current reader, if one is open."""
        ...

    def read_pulse(self) -> bool:
        """Read the next pulse and return whether one was available."""
        ...

    def read_waves(self) -> bool:
        """Read waveform samples for the current pulse."""
        ...

    def seek(self, index: int) -> bool:
        """Seek to pulse ``index``."""
        ...

    def get_format(self) -> int:
        """Return the PulseWaves format identifier for the open file."""
        ...

    def get_n_pulses(self) -> int:
        """Return the number of pulses advertised by the open reader."""
        ...

    def get_header(self) -> PulseHeader:
        """Return the open reader's header."""
        ...

    def get_pulse(self) -> Pulse:
        """Return the most recently read pulse."""
        ...


class PulseWriter:
    """Write pulses to PulseWaves-compatible output files."""

    def __init__(self) -> None: ...

    def open(self, filename: str, header: PulseHeader) -> bool:
        """Open ``filename`` for writing with ``header`` metadata."""
        ...

    def open_with_args(self, arguments: list[str], header: PulseHeader) -> bool:
        """Open a writer configured by ``PULSEwriteOpener`` argument strings."""
        ...

    def close(self, update_npulses: bool = True) -> None:
        """Close the writer and optionally update the pulse count."""
        ...

    def write_pulse(self, pulse: Pulse) -> bool:
        """Write one ``pulse`` to the open file."""
        ...

    def update_header(
        self,
        header: PulseHeader,
        use_inventory: bool = True,
        update_extra_bytes: bool = False,
    ) -> bool:
        """Rewrite header metadata for the open file."""
        ...

    def get_current_offset(self) -> int:
        """Return the current byte offset in the open writer."""
        ...


def version() -> str:
    """Return a human-readable PulseWaves library version string."""
