
#include "editorFrame.h"
#include "MidiC.h"
#include "MidiEvent.h"
#include "MidiNote.h"
#include "MidiTrack.h"


#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <wx/sizer.h>
#include <wx/stream.h>
#include <fstream>

EditorFrame::EditorFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name, MidiFile* midi) : wxDialog(parent, id, title, pos, size, style, name)
{
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	
	
	
	wxPanel* piano = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(200,100));

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

	editorPanel->SetScrollRate(FromDIP(10), FromDIP(10));
	
	editorPanel->SetSize(FromDIP(2000), FromDIP(2000));
	sizer->Add(piano, 0, wxEXPAND | wxALL, 5);
	sizer->Add(editorPanel, 2, wxEXPAND | wxALL, 5);

	


	this->SetSize(FromDIP(800), FromDIP(500));
	//editorPanel->SetMinSize({ FromDIP(400), FromDIP(200) });
	this->SetSizerAndFit(sizer);
	
	trackNumber = midi->currentTrack;
	editorPanel->SetBackgroundColour(wxColor(70, 70, 70));
	
	editorPanel->Bind(wxEVT_LEFT_DCLICK, &EditorFrame::OnDoubleClick, this);
	
	
	editorPanel->FlipGridFlag();
	DrawMIDIEvents(trackNumber, midi);
	//editorPanel->FlipGridFlag();
	
	
}

void EditorFrame::OnDoubleClick(wxMouseEvent& evt)
{
	auto position = evt.GetPosition();
	
	editorPanel->addNote(this->FromDIP(10), this->FromDIP(10), position.x, position.y, wxColor(255, 255, 255));
}

void EditorFrame::DrawMIDIEvents(int trackNumber, MidiFile* midi)
{
	editorPanel->SetTempo(midi->m_nBPM);
	auto& track = midi->vecTracks[trackNumber];
	
	if (!track.vecNotes.empty())
	{
		int timePerColumn = midi->m_nBPM/16;
		int noteHeight = 17;
		uint32_t noteRange = track.nMaxNote - track.nMinNote;

		float trackOffset = -170;
		for (auto& note : track.vecNotes)
		{

			editorPanel->addNote(note.nDuration / timePerColumn -2, noteHeight, std::round(((note.nStartTime - trackOffset) / timePerColumn)/(midi->m_nBPM/16)) * (midi->m_nBPM / 16), (std::round((noteRange - (note.nKey - track.nMinNote)) * noteHeight)/20)*20, wxColor(255, 255, 255));

		}
	}
}




/*
std::string EditorFrame::ResolveImagePath(std::string sImagePath) const
{
	namespace fs = std::filesystem;

	std::vector<fs::path> candidates = {
		fs::path(sImagePath),
		fs::path("./") / sImagePath,
		fs::path("../") / sImagePath,
		fs::path("../../") / sImagePath
	};

	for (const auto& p : candidates)
	{
		if (fs::exists(p))
			return p.string();
	}

	return sImagePath;
}
*/