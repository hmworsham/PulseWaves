/*
 * pulse2pulse - Convert between pulse waveform file formats
 *
 * Simplified implementation for Mac/Linux based on PulseWaves library
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pulsereader.hpp"
#include "pulsewriter.hpp"
#include "pulseutility.hpp"

void usage()
{
    fprintf(stderr, "Usage: pulse2pulse -i <input> -o <output> [options]\n");
    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, "  -i <file>     Input pulse file\n");
    fprintf(stderr, "  -o <file>     Output pulse file\n");
    fprintf(stderr, "  -verbose      Show progress information\n");
    fprintf(stderr, "  -h            Show this help\n");
    fprintf(stderr, "\nSupported formats:\n");
    fprintf(stderr, "  Input:  .pls, .plz, .lgw, .lgc\n");
    fprintf(stderr, "  Output: .pls, .txt\n");
}

int main(int argc, char *argv[])
{
    bool verbose = false;

    PULSEreadOpener pulsereadopener;
    PULSEwriteOpener pulsewriteopener;

    // Parse arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            usage();
            return 0;
        }
        else if (strcmp(argv[i], "-verbose") == 0) {
            verbose = true;
        }
    }

    // Parse file options
    pulsereadopener.parse(argc, argv);
    pulsewriteopener.parse(argc, argv);

    if (!pulsereadopener.active()) {
        fprintf(stderr, "ERROR: No input file specified\n\n");
        usage();
        return 1;
    }

    if (!pulsewriteopener.active()) {
        fprintf(stderr, "ERROR: No output file specified\n\n");
        usage();
        return 1;
    }

    // Open pulse reader
    PULSEreader* pulsereader = pulsereadopener.open();

    if (!pulsereader) {
        fprintf(stderr, "ERROR: Could not open input file\n");
        return 1;
    }

    if (verbose) {
        printf("Reading from: %s\n", pulsereadopener.get_file_name());
        printf("Number of pulses: %lld\n", pulsereader->header.number_of_pulses);
    }

    // Open pulse writer
    PULSEwriter* pulsewriter = pulsewriteopener.open(&pulsereader->header);

    if (!pulsewriter) {
        fprintf(stderr, "ERROR: Could not open output file\n");
        delete pulsereader;
        return 1;
    }

    if (verbose) {
        printf("Writing to: %s\n", pulsewriteopener.get_file_name());
    }

    // Copy pulses
    long long count = 0;
    long long progress_step = pulsereader->header.number_of_pulses / 100;
    if (progress_step == 0) progress_step = 1;

    while (pulsereader->read_pulse()) {
        pulsewriter->write_pulse(&pulsereader->pulse);
        count++;

        if (verbose && (count % progress_step == 0)) {
            int percent = (int)((count * 100) / pulsereader->header.number_of_pulses);
            printf("\rProgress: %d%% (%lld/%lld pulses)",
                   percent, count, pulsereader->header.number_of_pulses);
            fflush(stdout);
        }
    }

    if (verbose) {
        printf("\rProgress: 100%% (%lld/%lld pulses)\n", count, count);
        printf("Conversion complete!\n");
    }

    // Update header
    pulsewriter->update_header(&pulsereader->header);

    pulsereader->close();
    pulsewriter->close();

    delete pulsereader;
    delete pulsewriter;

    return 0;
}
