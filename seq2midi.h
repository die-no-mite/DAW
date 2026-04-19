
#include <thread>

class MidiPlayer
{
public:
	MidiPlayer();
	// open a stream, play a sequence, close the stream
	void seq_play(Alg_seq& seq);
	void pauseMidi();
private:
	// play a sequence to a midi stream
	void seq2midi(Alg_seq& seq, PortMidiStream* midi);
	std::jthread mainThread;

	bool isThreadActive = true;
	bool shouldDetach = true;
	PortMidiStream* mo;
	
};

