#pragma once

#include <wx/wx.h>

#include "graphicMIDIevent.h"

#include "midiFrame.h"

#include "MidiC.h"

class EditorFrame : public wxDialog
{
public:
	EditorFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name, MidiFile* midi);
	
	MidiFrame* editorPanel = new MidiFrame(this, wxID_ANY, wxDefaultPosition, wxSize(800, 500));

private:

	void OnDoubleClick(wxMouseEvent& evt);

	void DrawMIDIEvents(int trackNumber, MidiFile* midi);
	
	
	wxStaticBitmap* pianoImage;

	std::list<GraphicMIDIEvent> noteList;

	std::string ResolveImagePath(std::string sImagePath) const;

	const wxImage image;

	int trackNumber;

};