/*
 * pulseinfo - Display information about pulse waveform files
 *
 * Simplified implementation for Mac/Linux based on PulseWaves library
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pulsereader.hpp"
#include "pulseutility.hpp"

void usage()
{
    fprintf(stderr, "Usage: pulseinfo -i <input.pls> [options]\n");
    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, "  -i <file>     Input pulse file\n");
    fprintf(stderr, "  -verbose      Show detailed information\n");
    fprintf(stderr, "  -no_header    Skip header information\n");
    fprintf(stderr, "  -h            Show this help\n");
    fprintf(stderr, "\nSupported formats: .pls, .plz, .lgw, .lgc\n");
}

int main(int argc, char *argv[])
{
    bool verbose = false;
    bool show_header = true;

    PULSEreadOpener pulsereadopener;

    // Parse arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            usage();
            return 0;
        }
        else if (strcmp(argv[i], "-verbose") == 0) {
            verbose = true;
        }
        else if (strcmp(argv[i], "-no_header") == 0) {
            show_header = false;
        }
    }

    // Parse file options
    pulsereadopener.parse(argc, argv);

    if (!pulsereadopener.active()) {
        fprintf(stderr, "ERROR: No input file specified\n\n");
        usage();
        return 1;
    }

    // Open pulse reader
    PULSEreader* pulsereader = pulsereadopener.open();

    if (!pulsereader) {
        fprintf(stderr, "ERROR: Could not open pulse file\n");
        return 1;
    }

    // Display header information
    if (show_header) {
        printf("File: %s\n", pulsereadopener.get_file_name());
        printf("Number of pulses: %lld\n", pulsereader->header.number_of_pulses);
        printf("Bounding box:\n");
        printf("  X: [%.3f, %.3f]\n", pulsereader->header.min_x, pulsereader->header.max_x);
        printf("  Y: [%.3f, %.3f]\n", pulsereader->header.min_y, pulsereader->header.max_y);
        printf("  Z: [%.3f, %.3f]\n", pulsereader->header.min_z, pulsereader->header.max_z);

        // Skip verbose descriptor info for now
    }

    // Sample first few pulses if verbose
    if (verbose) {
        printf("\nFirst 10 pulses:\n");
        int count = 0;
        while (pulsereader->read_pulse() && count < 10) {
            pulsereader->pulse.compute_anchor();
            printf("  Pulse %d: (%.3f, %.3f, %.3f) intensity=%d\n",
                   count,
                   pulsereader->pulse.get_anchor_x(),
                   pulsereader->pulse.get_anchor_y(),
                   pulsereader->pulse.get_anchor_z(),
                   pulsereader->pulse.intensity);
            count++;
        }
    }

    pulsereader->close();
    delete pulsereader;

    return 0;
}
