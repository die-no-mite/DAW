#ifndef MIDINOTE_H
#define MIDINOTE_H

#include <cstdint>

struct MidiNote
{
    uint8_t nKey = 0;
    uint8_t nVelocity = 0;
    uint32_t nStartTime = 0;
    uint32_t nDuration = 0;
};

#endif