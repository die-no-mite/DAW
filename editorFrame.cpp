
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
	
	
	file.open("output.txt");
	
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
	editorPanel->Bind(UPDATE_NOTE, &EditorFrame::OnUpdateNote, this);
	editorPanel->Bind(FINISH_UPDATE_NOTE, &EditorFrame::FinishUpdateNote, this);
	
	editorPanel->SetTempo(midi->m_nBPM);
	editorPanel->FlipGridFlag();

	DrawMIDIEvents(trackNumber, midi);
	//editorPanel->FlipGridFlag();

	
	
}

void EditorFrame::OnDoubleClick(wxMouseEvent& evt)
{
	auto position = evt.GetPosition();
	
	editorPanel->addNote(this->FromDIP(10), this->FromDIP(10), position.x, position.y, wxColor(255, 255, 255));
}

void EditorFrame::OnUpdateNote(wxCommandEvent& evt)
{
	
	int currentID = editorPanel->GetCurrentID();
	bool foundFlag = false;
	targetIndex = 0;
	
	

	
	for (auto element : notesStored)
	{
		if (element.noteID = currentID)
		{
			foundFlag = true;
			break;
		}
		targetIndex++;
	}
	
	if (foundFlag)
	{
		newX = std::round(editorPanel->GetCoords().x / (editorPanel->GetTempo() / 8)) * (editorPanel->GetTempo() / 8);
		newY = std::round(editorPanel->GetCoords().y / 20) * 20;

		
		file << newX << " " << newY << "test" << std::endl;
		
		notesStored[targetIndex].x = newX;
		notesStored[targetIndex].y = newY;
		newDuration = notesStored[targetIndex].length;
	
	}
	// will remove the top note (the one that was being dragged) and redraw the note
	// at the snapped position, getting the width, height, and coordinates from
	// the vector that hasn't been created yet. The vector will be searched for the
	// correct ID to ensure that the correct note is being updated in the vector
}

void EditorFrame::FinishUpdateNote(wxCommandEvent& evt)
{
	int noteHeight = 17;

	editorPanel->removeTopNote();
	editorPanel->addNote(newDuration, noteHeight, newX, newY, wxColor(255, 255, 255), notesStored[targetIndex].noteID);
}

void EditorFrame::DrawMIDIEvents(int trackNumber, MidiFile* midi)
{
	
	auto& track = midi->vecTracks[trackNumber];
	int realDuration;
	int realX;
	int realY;
	bool tester = true;

	if (!track.vecNotes.empty())
	{
		double timePerColumn = 10;
		int noteHeight = 17;
		uint32_t noteRange = track.nMaxNote - track.nMinNote;

		float trackOffset = -170;
		for (auto& note : track.vecNotes)
		{
			realDuration = note.nDuration / timePerColumn - 4;
			realX = std::round(((note.nStartTime - trackOffset) / timePerColumn) / (editorPanel->GetTempo() / 8)) * (editorPanel->GetTempo() / 8);
			realY = std::round((noteRange - (note.nKey - track.nMinNote)) * noteHeight / 20) * 20;

			if (tester)
			{
			file << (note.nStartTime - trackOffset) / timePerColumn << " " << (noteRange - (note.nKey - track.nMinNote)) * noteHeight <<std::endl;
			file << realX << " " << realY << std::endl;
			tester = false;
			}

			LogNote(realX, realY, realDuration);
			editorPanel->addNote(realDuration, noteHeight, realX, realY, wxColor(255, 255, 255), notesStored[notesStored.size()-1].noteID);

		}
	}
}


void EditorFrame::OnClose(wxCloseEvent& event) 
{

}

void EditorFrame::LogNote(float xcoord, float ycoord, float len) {
	noteInfo newNote;
	newNote.noteID = giveID;
	newNote.x = xcoord;
	newNote.y = ycoord;
	newNote.length = len;
	
	giveID += 1;

	notesStored.push_back(newNote);//copy is pushed
}


void EditorFrame::LogMidiData() {
	//MIDI DATA UPDATING GOES HERE, GET DATA FROM notesStored
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