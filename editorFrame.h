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
	wxPanel* piano;

	wxPoint GetNewMinMax();

private:
	int GetGridStepX() const;
	int GetGridStepY() const;
	int SnapValue(int rawValue, int origin, int step) const;
	int SnapX(int rawX);
	int SnapY(int rawY);
	void EnsureGridAnchor(int rawX, int rawY);
	void ResetGridAnchorIfEmpty();

	void OnDoubleClick(wxMouseEvent& evt);
	void OnUpdateNote(wxCommandEvent& evt);
	void FinishUpdateNote(wxCommandEvent& evt);
	void OnRemoveNote(wxCommandEvent& evt);
	void ButtonPress(wxKeyEvent& event);
	void OnPaint(wxPaintEvent& event);
	void sendUpdateTrack();

	void OnClose(wxCloseEvent& evt);
	void OnRelativePostitionEvent(wxCommandEvent& evt);

	void DrawMIDIEvents(int trackNumber);
	void DrawNoteLabels(int trackNumber, int key);
	void GetNoteName(int key);
	void CheckNewHighestLowest(int y);


	MidiFile* midi;

	std::list<GraphicMIDIEvent> noteList;

	std::string ResolveImagePath(std::string sImagePath) const;


	int trackNumber;
	std::ofstream file;

	Alg_seq_ptr seq;

	int newX, newY, newDuration;
	int targetIndex = 0;
	float relativePosition;
	bool relativePositionFlag;
	bool noteLabelsDrawn = false;
	wxString firstNoteLabel;
	int firstNoteY;
	std::string noteName;
	int octave;
	int noteNumber;
	int firstOctave;
	int firstNoteNumber;

	int highestY = 0;
	int lowestY = 10000;
	int distanceToCeilingFloor;
	int newMin, newMax;

	bool ceilingReached = false;
	bool floorReached = false;

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

	bool hasGridAnchor = false;
	int gridAnchorX = 0;
	int gridAnchorY = 0;

};
