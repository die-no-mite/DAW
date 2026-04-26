#pragma once

#include <thread>
#include <atomic>
#include <memory>

class Alg_seq;
typedef Alg_seq* Alg_seq_ptr;
typedef void PortMidiStream;

class MidiPlayer
{
public:
    MidiPlayer();
    ~MidiPlayer();

    void seq_play(Alg_seq_ptr seq);
    void pauseMidi();

private:
    void seq2midi(Alg_seq& seq, PortMidiStream* midi, std::stop_token st);

    std::jthread mainThread;
    std::atomic<bool> isThreadActive{ false };

    PortMidiStream* mo = nullptr;

    Alg_seq_ptr currentSeq = nullptr;
    std::unique_ptr<Alg_seq> playbackSeq;
};