
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

EditorFrame::EditorFrame() //: wxDialog(parent, id, title, pos, size, style, name)
{

}

EditorFrame::EditorFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name, MidiFile* midifile) : wxDialog(parent, id, title, pos, size, style, name)
{
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);

	midi = midifile;
		
	wxPanel* piano = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(200,100));

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

	editorPanel = new MidiFrame(this, wxID_ANY, wxDefaultPosition, wxSize(800, 500));

	editorPanel->SetScrollRate(FromDIP(10), FromDIP(10));
	
	editorPanel->SetSize(FromDIP(2000), FromDIP(2000));
	sizer->Add(piano, 0, wxEXPAND | wxALL, 5);
	sizer->Add(editorPanel, 2, wxEXPAND | wxALL, 5);

	


	this->SetSize(FromDIP(800), FromDIP(500));
	//editorPanel->SetMinSize({ FromDIP(400), FromDIP(200) });
	this->SetSizerAndFit(sizer);
	this->Bind(wxEVT_CLOSE_WINDOW, &EditorFrame::OnClose, this);
	
	trackNumber = midi->currentTrack;
	editorPanel->SetBackgroundColour(wxColor(70, 70, 70));
	
	editorPanel->Bind(wxEVT_LEFT_DCLICK, &EditorFrame::OnDoubleClick, this);
	editorPanel->Bind(UPDATE_NOTE, &EditorFrame::OnUpdateNote, this);
	editorPanel->Bind(FINISH_UPDATE_NOTE, &EditorFrame::FinishUpdateNote, this);
	
	editorPanel->SetTempo(midi->m_nBPM);
	editorPanel->FlipGridFlag();

	DrawMIDIEvents(trackNumber);
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
		
		notesStored[targetIndex].x = newX;
		notesStored[targetIndex].y = newY;
		newDuration = notesStored[targetIndex].length;
	
	}
	
}

void EditorFrame::FinishUpdateNote(wxCommandEvent& evt)
{
	int noteHeight = 17;

	editorPanel->removeTopNote();
	editorPanel->addNote(newDuration, noteHeight, newX, newY, wxColor(255, 255, 255), notesStored[targetIndex].noteID);
}


void EditorFrame::sendUpdateTrack()
{
	TrackUpdateEvent event(EVT_UPDATE_TRACK);

	event.SetTrackNumber(trackNumber);
	event.SetEventObject(this);

	ProcessWindowEvent(event);
}



void EditorFrame::DrawMIDIEvents(int trackNumber)
{

	auto& track = midi->vecTracks[trackNumber];
	int realDuration;
	int realX;
	int realY;

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


			LogNote(realX, realY, realDuration);
			editorPanel->addNote(realDuration, noteHeight, realX, realY, wxColor(255, 255, 255), notesStored[notesStored.size()-1].noteID);

		}
	}
}


void EditorFrame::OnClose(wxCloseEvent& event) 
{
	
	for (int i = 0; i < notesStored.size() - 1; i++) 
	{
		for (int j = 0; j < notesStored.size() - i - 1; j++) 
		{
			if (notesStored[j].x > notesStored[j + 1].x) 
			{
				auto temp = notesStored[j];
				notesStored[j] = notesStored[j + 1];
				notesStored[j + 1] = temp;
			
			}
		}
	}
	LogMidiData();
	sendUpdateTrack();

	Destroy();
}



void EditorFrame::LogNote(float xcoord, float ycoord, float len) 
{
	noteInfo newNote;
	newNote.noteID = giveID;
	newNote.x = xcoord;
	newNote.y = ycoord;
	newNote.length = len;
	
	giveID += 1;

	notesStored.push_back(newNote);//copy is pushed
}


void EditorFrame::LogMidiData() 
{
	auto& currentTrack = midi->vecTracks[trackNumber];  
	uint32_t noteRange = currentTrack.nMaxNote - currentTrack.nMinNote;
	auto& minNote = currentTrack.nMinNote;
	currentTrack.vecNotes.clear();
	auto& noteVector = currentTrack.vecNotes;
	for (auto& note : notesStored)
	{
		MidiNote noteToAdd;
		noteToAdd.nStartTime = note.x + -170 * 10; // -170 is the trackoffset, *10 is the time per column
		noteToAdd.nDuration = note.length * 10 + 4;
		noteToAdd.nKey = (-noteRange - (note.y/17) + minNote);
		noteVector.push_back(noteToAdd);
	}
	midi->vecTracks[trackNumber].vecNotes = noteVector;
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
