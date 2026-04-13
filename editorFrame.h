#pragma once

#include <wx/wx.h>

#include "graphicMIDIevent.h"
#include "midiFrame.h"
#include "trackUpdateEvent.h"
#include "MidiC.h"

#include "allegro.h"

class EditorFrame : public wxDialog
{
public:
	EditorFrame();
	EditorFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name, MidiFile* midi, Alg_seq_ptr algseq);

	MidiFrame* editorPanel;
	

private:

	void OnDoubleClick(wxMouseEvent& evt);
	void OnUpdateNote(wxCommandEvent& evt);
	void FinishUpdateNote(wxCommandEvent& evt);
	void OnRemoveNote(wxCommandEvent& evt);
	void sendUpdateTrack();

	void OnClose(wxCloseEvent& evt);
	void OnRelativePostitionEvent(wxCommandEvent& evt);

	void DrawMIDIEvents(int trackNumber);

	wxStaticBitmap* pianoImage;

	MidiFile* midi;

	std::list<GraphicMIDIEvent> noteList;

	std::string ResolveImagePath(std::string sImagePath) const;

	const wxImage image;

	int trackNumber;
	std::ofstream file;
	int var = 25;

	Alg_seq_ptr seq;

	int newX, newY, newDuration;
	int targetIndex = 0;
	float relativePosition;

	void LogNote(float xcoord, float ycoord, float len);

	void LogMidiData();

	struct noteInfo
	{
		int noteID;
		float x;           //when played
		float y;           //pitch
		float length;      //duration

	};

	int giveID = 1;
	std::vector<noteInfo> notesStored; //stores note info here
	float placeholderLength = 10;

	std::vector<int> notesToRemove;

};
