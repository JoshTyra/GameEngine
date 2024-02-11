#pragma once

// Declare any public functions from stb_vorbis that you use here
// For example:
extern "C" {
    int stb_vorbis_decode_filename(const char* filename, int* channels, int* sample_rate, short** output);
    // ... and other used functions declarations
}
