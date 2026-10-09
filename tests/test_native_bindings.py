import unittest
from pathlib import Path


try:
    from pulsewaves import pulsewaves_native as pw
except ImportError:
    pw = None


DATA_DIR = Path(__file__).resolve().parents[1] / "data"


@unittest.skipIf(pw is None, "pulsewaves_native is not built")
class NativeBindingsTest(unittest.TestCase):
    def test_header_descriptor_scanner_and_reader_smoke(self):
        sampling = pw.PulseSampling()
        sampling.description = "returning"

        composition = pw.PulseComposition()
        composition.number_of_samplings = 1

        header = pw.PulseHeader()
        self.assertTrue(header.add_descriptor(composition, [sampling], 1))
        self.assertEqual(header.get_descriptor_composition(1).number_of_samplings, 1)
        self.assertEqual(len(header.get_descriptor_samplings(1)), 1)

        scanner = pw.PulseScanner()
        scanner.instrument = "unit-test"
        self.assertTrue(header.add_scanner(scanner, 1))
        self.assertEqual(header.get_scanner(1).instrument, "unit-test")

        reader = pw.PulseReader()
        self.assertTrue(reader.open(str(DATA_DIR / "test.pls")))
        self.assertGreater(reader.get_header().number_of_pulses, 0)
        self.assertTrue(reader.read_pulse())
        pulse = reader.get_pulse()
        header = reader.get_header()
        self.assertEqual(pulse.get_anchor_x(), header.get_x(pulse.anchor_X))
        self.assertEqual(pulse.get_target_x(), header.get_x(pulse.target_X))
        reader.close()

    def test_utility_bindings_smoke(self):
        quantizer = pw.PulseQuantizer()
        self.assertEqual(quantizer.get_x(5), 0.05)

        pulse_zip = pw.PulseZip()
        self.assertTrue(pulse_zip.setup(0, 0, 48))
        self.assertEqual(pulse_zip.num_items, 1)

        histogram = pw.PulseHistogram()
        self.assertTrue(histogram.histo("intensity", 1.0))
        self.assertTrue(histogram.histo("target_x", 1.0))

        filter_ = pw.PulseFilter()
        self.assertTrue(filter_.parse(["-keep_intensity", "0", "255"]))
        self.assertTrue(filter_.active())

        transform = pw.PulseTransform()
        self.assertTrue(transform.parse(["-translate_intensity", "1"]))
        self.assertTrue(transform.active())

        bin_ = pw.PulseBin(1.0)
        bin_.add_int(3)
        bin_.add_int64(4)
        bin_.add_float(5.5)
        bin_.add_value(1, 10)

        bin_snapshot = bin_.snapshot()
        self.assertEqual(bin_snapshot["count"], 4)
        self.assertEqual([entry["bin"] for entry in bin_snapshot["bins"]], [1, 3, 4, 5])

        header = pw.PulseHeader()
        pulse = pw.Pulse()
        self.assertTrue(pulse.init(header))
        pulse.intensity = 7
        pulse.set_anchor_and_target([0.0, 0.0, 0.0], [3.0, 0.0, 0.0])
        histogram.add(pulse)
        histogram_snapshot = histogram.snapshot()
        self.assertEqual(histogram_snapshot["intensity"]["bins"][0]["count"], 1)
        self.assertEqual(histogram_snapshot["target_x"]["bins"][0]["minimum"], 3.0)


if __name__ == "__main__":
    unittest.main()
