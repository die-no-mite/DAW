#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include <cstdint>

struct MidiEvent
{
    enum class Type
    {
        NoteOff,
        NoteOn,
        Other,
        Program
    } event;

    uint8_t nKey = 0;
    uint8_t nVelocity = 0;
    uint32_t nDeltaTick = 0;
    uint8_t nChannel = 0;
    uint8_t nProgram = 0;
};

#endif