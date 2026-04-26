
#include "editorFrame.h"
#include "MidiC.h"
#include "MidiEvent.h"
#include "MidiNote.h"
#include "MidiTrack.h"

#include "allegro.h"

#include <wx/graphics.h>
#include <wx/dcbuffer.h>
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

	piano = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(200, 100));
	piano->SetBackgroundStyle(wxBG_STYLE_PAINT);

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

	editorPanel = new MidiFrame(this, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(800), FromDIP(500)));

	editorPanel->SetVirtualSize(FromDIP(20000), FromDIP(3000));

	sizer->Add(piano, 0, wxEXPAND | wxALL, 5);
	sizer->Add(editorPanel, 2, wxEXPAND | wxALL, 5);

	this->SetSize(FromDIP(800), FromDIP(500));
	this->SetSizerAndFit(sizer);
	this->Bind(wxEVT_CLOSE_WINDOW, &EditorFrame::OnClose, this);

	trackNumber = midi->currentTrack;
	editorPanel->SetBackgroundColour(wxColor(70, 70, 70));

	editorPanel->Bind(wxEVT_LEFT_DCLICK, &EditorFrame::OnDoubleClick, this);
	editorPanel->Bind(UPDATE_NOTE, &EditorFrame::OnUpdateNote, this);
	editorPanel->Bind(FINISH_UPDATE_NOTE, &EditorFrame::FinishUpdateNote, this);
	editorPanel->Bind(REMOVED_NOTE, &EditorFrame::OnRemoveNote, this);
	editorPanel->Bind(RELATIVE_POSITION, &EditorFrame::OnRelativePostitionEvent, this);
	editorPanel->Bind(wxEVT_KEY_DOWN, &EditorFrame::ButtonPress, this);
	piano->Bind(wxEVT_PAINT, &EditorFrame::OnPaint, this);

	editorPanel->SetDivision(midi->nDivision);
	editorPanel->FlipGridFlag();

	DrawMIDIEvents(trackNumber);
	seq->convert_to_beats();

	newMin = midi->vecTracks[trackNumber].nMinNote;
	newMax = midi->vecTracks[trackNumber].nMaxNote;
}

void EditorFrame::ButtonPress(wxKeyEvent& event)
{
	if (event.GetKeyCode() == WXK_LEFT)
	{
		editorPanel->ShiftNotes('l', GetGridStepX(), GetGridStepY());
		relativePositionFlag = true;
	}
	else if (event.GetKeyCode() == WXK_RIGHT)
	{
		editorPanel->ShiftNotes('r', GetGridStepX(), GetGridStepY());
		relativePositionFlag = true;
	}
	else if (event.GetKeyCode() == WXK_UP)
	{
		if (!ceilingReached)
		{
			distanceToCeilingFloor++;
			editorPanel->ShiftNotes('u', GetGridStepX(), GetGridStepY());
			relativePositionFlag = true;
		}
	}
	else if (event.GetKeyCode() == WXK_DOWN)
	{
		if (!floorReached)
		{
			distanceToCeilingFloor--;
			editorPanel->ShiftNotes('d', GetGridStepX(), GetGridStepY());
			relativePositionFlag = true;
		}
	}
	Refresh();
}

int EditorFrame::GetGridStepX() const
{
	return std::max(1, (midi->nDivision / 20));
}

int EditorFrame::GetGridStepY() const
{
	return 17;
}

int EditorFrame::SnapValue(int rawValue, int origin, int step) const
{
	if (step <= 0)
	{
		return rawValue;
	}

	const double offset = static_cast<double>(rawValue - origin) / static_cast<double>(step);
	return origin + static_cast<int>(std::round(offset)) * step;
}

void EditorFrame::EnsureGridAnchor(int rawX, int rawY)
{
	if (!hasGridAnchor)
	{
		hasGridAnchor = true;
		gridAnchorX = rawX;
		gridAnchorY = rawY;
		if (editorPanel)
		{
			editorPanel->SetGridAnchor(gridAnchorX, gridAnchorY);
		}
	}
}

int EditorFrame::SnapX(int rawX)
{
	EnsureGridAnchor(rawX, gridAnchorY);
	return SnapValue(rawX, gridAnchorX, GetGridStepX());
}

int EditorFrame::SnapY(int rawY)
{
	EnsureGridAnchor(gridAnchorX, rawY);
	return SnapValue(rawY, gridAnchorY, GetGridStepY());
}

void EditorFrame::ResetGridAnchorIfEmpty()
{
	if (notesStored.empty())
	{
		hasGridAnchor = false;
		gridAnchorX = 0;
		gridAnchorY = 0;
		if (editorPanel)
		{
			editorPanel->ClearGridAnchor();
		}
	}
}

void EditorFrame::OnDoubleClick(wxMouseEvent& evt)
{
	evt.Skip();

	const auto position = evt.GetPosition();

	int worldX = 0;
	int worldY = 0;
	editorPanel->CalcUnscrolledPosition(position.x, position.y, &worldX, &worldY);

	const int logicalX = worldX - (editorPanel->GetShiftX());
	const int logicalY = worldY - (editorPanel->GetShiftY());

	EnsureGridAnchor(logicalX, logicalY);

	const int snappedX = SnapX(logicalX);
	const int snappedY = SnapY(logicalY);

	CheckNewHighestLowest(snappedY);

	editorPanel->addNote(FromDIP(10), 17, snappedX, snappedY, wxColor(255, 255, 255), giveID);
	LogNote(static_cast<float>(snappedX), static_cast<float>(snappedY), static_cast<float>(FromDIP(10)));

	
}

void EditorFrame::CheckNewHighestLowest(int y)
{
	int currentMax = 0;
	int currentMin = 100000;
	int distance;
	auto& currentTrack = midi->vecTracks[trackNumber];

	for (int i = 0; i < notesStored.size(); i++)
	{
		if (notesStored[i].y < currentMin)
			currentMin = notesStored[i].y;

		if (notesStored[i].y > currentMax)
			currentMax = notesStored[i].y;
	}

	if (y > currentMax)
	{
		distance = y - currentMax;
		newMin += (distance / GetGridStepY()) - 2;
	}
	else if (y < currentMin)
	{
		distance = currentMin - y;
		newMax += (distance / GetGridStepY()) - 6;
	}

}

void EditorFrame::OnUpdateNote(wxCommandEvent& evt)
{
	const int currentID = editorPanel->GetCurrentID();
	bool foundFlag = false;
	targetIndex = 0;

	for (const auto& element : notesStored)
	{
		if (element.noteID == currentID)
		{
			foundFlag = true;
			break;
		}
		++targetIndex;
	}

	if (!foundFlag || targetIndex >= static_cast<int>(notesStored.size()))
	{
		return;
	}

	if (!editorPanel->shouldExtend)
	{
		const int rawX = static_cast<int>(editorPanel->GetShiftedCoords().x - relativePosition);
		const int rawY = static_cast<int>(editorPanel->GetShiftedCoords().y);

		newX = SnapX(rawX);
		newY = SnapY(rawY);

		notesStored[targetIndex].x = static_cast<float>(newX);
		notesStored[targetIndex].y = static_cast<float>(newY);
		newDuration = static_cast<int>(notesStored[targetIndex].length);
	}
	else
	{
		newX = static_cast<int>(notesStored[targetIndex].x);
		newY = static_cast<int>(notesStored[targetIndex].y);

		const float durationCandidate = static_cast<float>(editorPanel->GetShiftedCoords().x) - static_cast<float>(newX);
		if (durationCandidate > 0.0f)
		{
			newDuration = std::max(GetGridStepX(), SnapValue(static_cast<int>(durationCandidate), 0, GetGridStepX()));
			notesStored[targetIndex].length = static_cast<float>(newDuration);
		}
		else
		{
			newDuration = static_cast<int>(notesStored[targetIndex].length);
		}
	}
}

void EditorFrame::FinishUpdateNote(wxCommandEvent& evt)
{
	constexpr int noteHeight = 17;
	const int noteID = notesStored[targetIndex].noteID;

	CheckNewHighestLowest(newY);

	editorPanel->removeNoteByID(noteID);
	editorPanel->addNote(newDuration, noteHeight, newX, newY, wxColor(255, 255, 255), noteID);
}

// triggered when user clicks on a note, gets the distance between the left side of the note
// (the x coordinate) and the mouse position for use during snapping
void EditorFrame::OnRelativePostitionEvent(wxCommandEvent& evt)
{
	const double xCoord = editorPanel->GetShiftedCoords().x;

	relativePositionFlag = false;
	const int currentID = editorPanel->GetCurrentID();
	targetIndex = 0;

	for (const auto& element : notesStored)
	{
		if (element.noteID == currentID)
		{
			relativePosition = static_cast<float>(xCoord - notesStored[targetIndex].x);
			return;
		}
		++targetIndex;
	}
}


void EditorFrame::OnRemoveNote(wxCommandEvent& evt)
{
	
	targetIndex = 0;
	for (const auto& element : notesStored)
	{
		if (element.noteID == evt.GetInt())
		{
			notesStored.erase(notesStored.begin() + targetIndex);
			ResetGridAnchorIfEmpty();
			return;
		}
		++targetIndex;
	}
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
	double realDuration;
	int realX;
	int realY;

	if (!track.vecNotes.empty())
	{
		double timePerColumn = 10;
		int noteHeight = 17;
		uint32_t noteRange = track.nMaxNote - track.nMinNote;
		int realRange = noteRange;
		float trackOffset = -170;
		int i = 0;
		for (auto& note : track.vecNotes)
		{
			const int realDuration = static_cast<int>(note.nDuration / timePerColumn);
			const int rawX = static_cast<int>((note.nStartTime - trackOffset) / timePerColumn);
			const int rawY = static_cast<int>((noteRange - (note.nKey - track.nMinNote)) * noteHeight);

			EnsureGridAnchor(rawX, rawY);

			const int realX = SnapX(rawX);
			const int realY = SnapY(rawY);

			if (note.nKey < lowestY)
				lowestY = note.nKey;

			if (note.nKey > highestY)
				highestY = note.nKey;

			if (!noteLabelsDrawn)
				DrawNoteLabels(realY, note.nKey);

			LogNote(static_cast<float>(realX), static_cast<float>(realY), static_cast<float>(realDuration));
			editorPanel->addNote(realDuration, noteHeight, realX, realY, wxColor(255, 255, 255),
				notesStored.back().noteID);
			
		}

	}
}

void EditorFrame::DrawNoteLabels(int coord, int key)
{
	firstNoteY = coord;
	firstOctave = 0;
	noteName;
	distanceToCeilingFloor = key;
	
	while (key >= 12)
	{
		key = key - 12;
		firstOctave++;
	}
	GetNoteName(key);
	firstNoteNumber = key;
	firstNoteLabel = noteName + std::to_string(firstOctave);
	Refresh();
}

void EditorFrame::GetNoteName(int y)
{
	switch (y)
	{
	case 0:
		noteName = "C";
		break;
	case 1:
		noteName = "Db";
		break;
	case 2:
		noteName = "D";
		break;
	case 3:
		noteName = "Eb";
		break;
	case 4:
		noteName = "E";
		break;
	case 5:
		noteName = "F";
		break;
	case 6:
		noteName = "Gb";
		break;
	case 7:
		noteName = "G";
		break;
	case 8:
		noteName = "Ab";
		break;
	case 9:
		noteName = "A";
		break;
	case 10:
		noteName = "Bb";
		break;
	case 11:
		noteName = "B";
		break;
	}
	
}

void EditorFrame::OnPaint(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc(piano);
	PrepareDC(dc);
	dc.Clear();

	wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
	if (!gc)
	{
		return;
	}

	wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	gc->SetFont(font, *wxBLACK);

	if (distanceToCeilingFloor == 127)
		ceilingReached = true;
	else if (distanceToCeilingFloor == 0)
		floorReached = true;
	else
	{
		ceilingReached = false;
		floorReached = false;
	}

		
	gc->DrawText(firstNoteLabel, 90, firstNoteY + editorPanel->GetShiftY());

	wxString noteLabel;
	int tempOctave = octave = firstOctave;
	int tempNoteNumber = noteNumber = firstNoteNumber;

	for (int y = firstNoteY - GetGridStepY(); (octave < 10 || noteNumber != 7); y -= GetGridStepY())
	{
		noteNumber++;
		if (noteNumber == 12)
		{
			noteNumber = 0;
			octave++;
		}
		GetNoteName(noteNumber);
		noteLabel = noteName + std::to_string(octave);
		gc->DrawText(noteLabel, 90, y + editorPanel->GetShiftY());
	
	}
	octave = tempOctave;
	noteNumber = tempNoteNumber;
	//76 and 51
	for (int y = firstNoteY + GetGridStepY(); (octave > 0 || noteNumber != 0); y += GetGridStepY())
	{
		noteNumber--;
		if (noteNumber == -1)
		{
			noteNumber = 11;
			octave--;
		}
		GetNoteName(noteNumber);
		noteLabel = noteName + std::to_string(octave);
		gc->DrawText(noteLabel, 90, y + editorPanel->GetShiftY());
		
	}

	noteLabelsDrawn = true;
	delete gc;
}

void EditorFrame::OnClose(wxCloseEvent& event) 
{
	
	for (int i = 0; i < notesStored.size() - 1; i++) 
	{
		for (int j = 0; j < notesStored.size() - i - 1; j++) 
		{
			if (notesStored[j].x > notesStored[j + 1].x) 
			{
				std::swap(notesStored[j], notesStored[j + 1]);
			
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
	std::vector<Alg_event_ptr> metaEvents;
	auto& currentTrack = midi->vecTracks[trackNumber];  

	uint32_t noteRange = currentTrack.nMaxNote - currentTrack.nMinNote;
	int realNoteRange = noteRange;
	auto& minNote = currentTrack.nMinNote;
	auto& maxNote = currentTrack.nMaxNote;
	bool isPercusion = currentTrack.isPercusion;
	currentTrack.vecNotes.clear();
	auto& noteVector = currentTrack.vecNotes;

	int minCoord;

	int timePerBeat = midi->m_nTempo;
	float timePerMeasure = timePerBeat * midi->timeSigNum;
	int beatOffset = 0;
	float newBeat;
	auto algtrack = seq->track(trackNumber + midi->trackIndexOffset);
	seq->convert_to_beats();

	int index = 0;
	
	seq->clear_track(trackNumber + midi->trackIndexOffset, 0.1, algtrack->last_note_off + 10, true);
	
	float microPerBeat = float(timePerBeat) / float(midi->nDivision);

	for (auto& note : notesStored)
	{
		MidiNote noteToAdd;
		noteToAdd.nStartTime = (note.x * 10 + -170); // -170 is the trackoffset, *10 is the time per column
		noteToAdd.nDuration = note.length * 10;
		if(-((note.y - noteRange * 17 - minNote * 17) / 17) >= minNote)
			noteToAdd.nKey = -((note.y - noteRange * 17 - minNote * 17) / 17);
		else
		{
			minCoord = noteRange * 17;
			int distance = note.y - minCoord;
			int gridDistance = distance / 17;
			noteToAdd.nKey = minNote - gridDistance;
		}
		noteVector.push_back(noteToAdd);
		int realKey = noteToAdd.nKey;
		
		
		if (noteToAdd.nStartTime != 0)
			newBeat =  float(float(noteToAdd.nStartTime) * microPerBeat)/ float(timePerMeasure) * 4;
		else
			newBeat = 0;
		

		algtrack->convert_to_beats();
		Alg_note *algNote;
		if (isPercusion)
			algNote = algtrack->create_note(newBeat, 9, noteToAdd.nKey, noteToAdd.nKey, 127, double(noteToAdd.nDuration) / midi->nDivision);
		else
			algNote = algtrack->create_note(newBeat, 0, noteToAdd.nKey, noteToAdd.nKey, 127, double(noteToAdd.nDuration)/midi->nDivision);
		
		algtrack->add(algNote);
	}
	file.close();
	midi->vecTracks[trackNumber].vecNotes = noteVector;
	
}

wxPoint EditorFrame::GetNewMinMax()
{
	wxPoint minMax(newMin, newMax);
	return minMax;
}

