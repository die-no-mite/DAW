#ifndef MIDIC_H
#define MIDIC_H

#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <algorithm>
#include <iostream>

struct MidiNote;
struct MidiTrack;
struct MidiEvent;

class MidiFile {
    public:
        enum EventName : uint8_t {
            VoiceNoteOff = 0x80,
            VoiceNoteOn = 0x90,
            VoiceAftertouch = 0xA0,
            VoiceControlChange = 0xB0,
            VoiceProgramChange = 0xC0,
            VoiceChannelPressure = 0xD0,
            VoicePitchBend = 0xE0,
            SystemExclusive = 0xF0,
        };

    enum MetaEventName : uint8_t
    {
        MetaSequence = 0x00,
        MetaText = 0x01,
        MetaCopyright = 0x02,
        MetaTrackName = 0x03,
        MetaInstrumentName = 0x04,
        MetaLyrics = 0x05,
        MetaMarker = 0x06,
        MetaCuePoint = 0x07,
        MetaChannelPrefix = 0x20,
        MetaEndOfTrack = 0x2F,
        MetaSetTempo = 0x51,
        MetaSMPTEOffset = 0x54,
        MetaTimeSignature = 0x58,
        MetaKeySignature = 0x59,
        MetaSequencerSpecific = 0x7F,
    };

    public:
        MidiFile();
        explicit MidiFile(const std::string& sFileName);

        void Clear();
        bool ParseFile(const std::string& sFileName);

    public:
        std::vector<MidiTrack> vecTracks;
        uint32_t m_nTempo = 0;
        uint32_t m_nBPM = 0;
};

#endif

