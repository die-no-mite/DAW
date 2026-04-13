
#include "editorFrame.h"
#include "MidiC.h"
#include "MidiEvent.h"
#include "MidiNote.h"
#include "MidiTrack.h"

#include "allegro.h"

#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <wx/sizer.h>
#include <wx/stream.h>
#include <fstream>

EditorFrame::EditorFrame() //: wxDialog(parent, id, title, pos, size, style, name)
{

}

EditorFrame::EditorFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name, MidiFile* midifile, Alg_seq_ptr algseq) : wxDialog(parent, id, title, pos, size, style, name)
{
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	seq = algseq;
	midi = midifile;
	file.open("output.txt");
		
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
	editorPanel->Bind(REMOVED_NOTE, &EditorFrame::OnRemoveNote, this);
	editorPanel->Bind(RELATIVE_POSITION, &EditorFrame::OnRelativePostitionEvent, this);
	
	editorPanel->SetTempo(midi->m_nBPM);
	editorPanel->FlipGridFlag();

	DrawMIDIEvents(trackNumber);	
	file << midi->m_nTempo << std::endl;
}

void EditorFrame::OnDoubleClick(wxMouseEvent& evt)
{
	auto position = evt.GetPosition();
	
	editorPanel->addNote(this->FromDIP(10), 17, std::round(position.x / ((editorPanel->GetTempo() + var) / 8)) * ((editorPanel->GetTempo() + var) / 8), std::round(position.y / 20) * 20, wxColor(255, 255, 255), giveID);
	LogNote(position.x, position.y, this->FromDIP(10));
}

void EditorFrame::OnUpdateNote(wxCommandEvent& evt)
{
	
	int currentID = editorPanel->GetCurrentID();
	bool foundFlag = false;
	targetIndex = 0;
	
	for (auto element : notesStored)
	{
		if (element.noteID == currentID)
		{
			foundFlag = true;
			break;
		}
		targetIndex++;
	}
	file << notesStored[targetIndex].length << std::endl;

	if (foundFlag && !editorPanel->shouldExtend)
	{
		newX = editorPanel->GetCoords().x - relativePosition;
		newX = std::round(newX / ((editorPanel->GetTempo() + var) / 8)) * ((editorPanel->GetTempo() + var) / 8);
		newY = std::round(editorPanel->GetCoords().y / 20) * 20;
		
		notesStored[targetIndex].x = newX;
		notesStored[targetIndex].y = newY;
		newDuration = notesStored[targetIndex].length;
	
	}
	else if (editorPanel->shouldExtend)
	{
		newX = notesStored[targetIndex].x;
		newY = notesStored[targetIndex].y;
		float durationCandidate = editorPanel->GetCoords().x - newX;
		file << durationCandidate << std::endl;
		if (durationCandidate > 0)
		{
			newDuration = std::round(durationCandidate / ((editorPanel->GetTempo() + var) / 8)) * ((editorPanel->GetTempo() + var) / 8);
			notesStored[targetIndex].length = newDuration;
		}
	}
	
}

void EditorFrame::FinishUpdateNote(wxCommandEvent& evt)
{
	int noteHeight = 17;
	file << newX << std::endl;
	editorPanel->removeTopNote();
	editorPanel->addNote(newDuration, noteHeight, newX, newY, wxColor(255, 255, 255), notesStored[targetIndex].noteID);
}

// triggered when user clicks on a note, gets the distance between the left side of the note
// (the x coordinate) and the mouse position for use during snapping
void EditorFrame::OnRelativePostitionEvent(wxCommandEvent& evt)
{
	auto xCoord = editorPanel->GetCoords().x;
	int currentID = editorPanel->GetCurrentID();
	bool foundFlag = false;
	targetIndex = 0;

	for (auto element : notesStored)
	{
		if (element.noteID == currentID)
		{
			foundFlag = true;
			break;
		}
		targetIndex++;
	}
	relativePosition = xCoord - notesStored[targetIndex].x;
}


void EditorFrame::OnRemoveNote(wxCommandEvent& evt)
{
	
	for (auto element : notesStored)
	{
		if (element.noteID == evt.GetInt())
		{
			break;
		}
		targetIndex++;
	}
	notesStored.erase(notesStored.begin() + targetIndex);
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
		int realRange = noteRange;
		float trackOffset = -170;

		for (auto& note : track.vecNotes)
		{
			realDuration = note.nDuration / timePerColumn;
			realX = std::round(((note.nStartTime - trackOffset) / timePerColumn) / ((editorPanel->GetTempo() + var) / 8)) * ((editorPanel->GetTempo() + var) / 8);
			//realX = ((note.nStartTime - trackOffset) / timePerColumn);
			realY = std::round((noteRange - (note.nKey - track.nMinNote)) * noteHeight / 20) * 20;
			int realKey = note.nKey;
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
	int realNoteRange = noteRange;
	auto& minNote = currentTrack.nMinNote;
	currentTrack.vecNotes.clear();
	auto& noteVector = currentTrack.vecNotes;

	int timePerBeat = midi->m_nTempo;
	float timePerMeasure = timePerBeat * midi->timeSigNum;
	int beatOffset = 0;
	float newBeat;
	auto algtrack = seq->track(trackNumber+3);
	algtrack->convert_to_beats();
	algtrack->clear(1, algtrack->last_note_off + 10, true);

	file << "time per beat: " << timePerBeat << std::endl;

	float microPerBeat = float(timePerBeat) / float(midi->nDivision);

	for (auto& note : notesStored)
	{
		MidiNote noteToAdd;
		noteToAdd.nStartTime = (note.x * 10 + -170) - 80; // -170 is the trackoffset, *10 is the time per column
		noteToAdd.nDuration = note.length * 10;
		noteToAdd.nKey = -((note.y - noteRange * 17 - minNote * 17) / 17);
		noteVector.push_back(noteToAdd);
		int realKey = noteToAdd.nKey;
		int realStart = noteToAdd.nStartTime / 1400; 
		
		file << "start time: " << noteToAdd.nStartTime;
		
		if (noteToAdd.nStartTime != 0)
			newBeat =  float(noteToAdd.nStartTime * microPerBeat * 4) / float(timePerMeasure);
		else
			newBeat = 0;
		file << " new beat: " << newBeat << std::endl;
		

		
		auto algNote = algtrack->create_note(newBeat, 0, 0, noteToAdd.nKey, 127, noteToAdd.nDuration);
		algtrack->add(algNote);
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
