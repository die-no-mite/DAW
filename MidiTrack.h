#ifndef MIDITRACK_H
#define MIDITRACK_H

#include <cstdint>

struct MidiNote;

struct MidiTrack
{
    std::string sName;
    std::string sInstrument;
    std::vector<MidiEvent> vecEvents;
    std::vector<MidiNote> vecNotes;
    uint8_t nMaxNote = 64;
    uint8_t nMinNote = 64;
};
#endif