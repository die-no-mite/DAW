

#include <fstream>
#include <array>
#include <vector>
#include <random>

#include "MidiC.h"
#include "MidiEvent.h"
#include "MidiNote.h"
#include "MidiTrack.h"
#include "midiFrame.h"
#include "ColorPane.h"



#include <wx/wx.h>
#include <wx/timer.h>
#include <wx/wrapsizer.h>
#include <wx/splitter.h>


//#pragma comment(lib, "winmm.lib")




class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
	void SetupInfoPanes(wxWindow* parent, wxSizer* sizer);
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	

private:
	void SetupPenPanes(wxWindow* parent, wxSizer* sizer);
	wxPanel *BuildTrackInfoPanel(wxWindow* parent);

	void OnAddButtonClick(wxCommandEvent &event);
	void OnRemoveButtonClick(wxCommandEvent &event);
	void OnMouseEvent(wxMouseEvent &event);

	void OnNoteAdded(wxCommandEvent& event);
	void OnNoteRemoved(wxCommandEvent& event);

	wxPanel* createButtonPanel(wxWindow* parent);

	

	MidiFrame *canvas;

	int rectCount = 0;
	std::mt19937 randomGen;


	const std::string lightBackground = "#f4f3f3";
	const std::string darkBackground = "#2c2828";
};
/*
wxPanel* MyFrame::BuildTrackInfoPanel(wxWindow* parent)
{
	std::vector<wxString> form = {
		{"Track 1"},
		{"Track 2"},
		{"Track 3"}
	};
}
*/

/*
class MIDIPane : public wxPanel {
public:

	MIDIPane(wxFrame* parent);

	void paintEvent(wxPaintEvent& evt);

	//void OnQuit(wxCommandEvent& event);
	//void OnAbout(wxCommandEvent& event);


	void paintNow();

	void render(wxDC& dc);

	wxDECLARE_EVENT_TABLE();



	//MIDIPane() : wxFrame(nullptr, wxID_ANY, "MIDI File Viewer", wxDefaultPosition, wxSize(1200, 800)), timer(this) {


	//SetBackgroundStyle(wxBG_STYLE_PAINT);


	//void OnTimer(wxTimerEvent& timer);

	//timer.Start(16);
	//}
private:



	MidiFile midi;
	wxTimer timer;


	size_t nCurrentNote[16]{};

	double dSongTime = 0.0;
	double dRunTime = 0.0;
	uint32_t nMidiClock = 0;

	float nTrackOffset = 1000.0f;

	void OnTimer(wxTimerEvent&) {
		Refresh(false);
	}
	void OnKeyDown(wxKeyEvent& key) {

		switch (key.GetKeyCode()) {
		case WXK_LEFT:
			nTrackOffset -= 100;
			break;
		case WXK_RIGHT:
			nTrackOffset += 100;
		default:
			key.Skip();
		}
	}



};
*/

enum
{
	Minimal_Quit = wxID_EXIT,
	Minimal_About = wxID_ABOUT
};



wxPanel* MyFrame::BuildTrackInfoPanel(wxWindow* parent)
{
	auto trackInfoPanel = new wxScrolled<wxPanel>(parent, wxID_ANY);
	trackInfoPanel->SetScrollRate(0, FromDIP(10));

	bool isDark = wxSystemSettings::GetAppearance().IsDark();
	trackInfoPanel->SetBackgroundColour(wxColor(isDark ? darkBackground : lightBackground));

	auto mainSizer = new wxBoxSizer(wxVERTICAL);

	auto text = new wxStaticText(trackInfoPanel, wxID_ANY, "Tracks");
	mainSizer->Add(text, 0, wxALL, FromDIP(5));

	auto infoPaneSizer = new wxWrapSizer(wxHORIZONTAL);
	SetupInfoPanes(trackInfoPanel, infoPaneSizer);

	mainSizer->Add(infoPaneSizer, 0, wxALL, FromDIP(5));

	mainSizer->AddStretchSpacer();
	
	mainSizer->AddSpacer(FromDIP(5));

	trackInfoPanel->SetSizer(mainSizer);


	return trackInfoPanel;
}



wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	

	MidiFile *midi = new MidiFile;

	size_t nCurrentNote[16]{ 0 };

	double dSongTime = 0.0;
	double dRunTime = 0.0;
	uint32_t nMidiClock = 0;
	
	MyFrame *frame = new MyFrame("DAW", wxDefaultPosition, wxDefaultSize);
	frame->Show(true);

	//wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);


	return true;
}


void MyFrame::SetupInfoPanes(wxWindow* parent, wxSizer* sizer)
{
	NULL;
}


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(nullptr, wxID_ANY, title, pos, size)
{
	MidiFile *midi = new MidiFile;
	midi->ParseFile("battle-theme.mid");

	//gets the number of tracks from the midi file and creates a vector (dynamic array) of panels of the size of the track number
	int trackNumber = midi->getTrackNum();
	std::vector<MidiFrame*> trackList;
	

	wxSplitterWindow *splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

	splitter->SetMinimumPaneSize(FromDIP(150));
	 
	auto trackInfoPanel = BuildTrackInfoPanel(splitter);
	canvas = new MidiFrame(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize);

	wxBoxSizer* trackAreaSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer* trackSizer = new wxBoxSizer(wxVERTICAL);

	trackAreaSizer->Add(splitter, 1, wxEXPAND, 0);
	trackAreaSizer->Add(canvas, 1, wxEXPAND | wxALL, 5);
	

	
	if (trackNumber == 1) //sets up track if there's only one track
	{
		trackList[0] = new MidiFrame(canvas, wxID_ANY, wxDefaultPosition, wxDefaultSize);
		trackList[0]->SetBackgroundColour(wxColor(0, 0, 0));
		trackSizer->Add(trackList[0], 1, wxEXPAND | wxALL, 5);
	}
	else 
	{
		//fills vector with midiframes per track and adds them to the sizer
		for (int i = 0; i < trackNumber; i++)
		{
			trackList[i] = new MidiFrame(canvas, wxID_ANY, wxDefaultPosition, wxDefaultSize);
			trackList[i]->SetBackgroundColour(wxColor(0,0,0));
			if (i == 0)
				trackSizer->Add(trackList[i], 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5); //first track only has a boarder on the left right and top
			else if (i == trackNumber - 1)
				trackSizer->Add(trackList[i], 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5); //last track only has a boarder on the left right and bottom
			else
				trackSizer->Add(trackList[i], 1, wxEXPAND | wxLEFT | wxRIGHT, 5); //middle tracks only have a boarder on the left and right
		}
	}
	
	canvas->SetSizerAndFit(trackSizer);

	splitter->SplitVertically(trackInfoPanel, canvas);
	splitter->SetSashPosition(FromDIP(220));

	this->SetSize(FromDIP(800), FromDIP(500));
	this->SetMinSize({ FromDIP(400), FromDIP(200) });
	
	
	
	
	canvas->Bind(CANVAS_RECT_ADDED, &MyFrame::OnNoteAdded, this);
	
	//these commented out lines (even the 3 at the bottom) are what was making the program window bug out, they'll need to be reimplemented at some point

	//wxSizer* sizer = new wxBoxSizer(wxVERTICAL);


	//auto buttonPanel = createButtonPanel(this);

	
	
	canvas->Bind(CANVAS_RECT_REMOVED, &MyFrame::OnNoteRemoved, this);
	canvas->Bind(wxEVT_LEFT_DCLICK, &MyFrame::OnMouseEvent, this);


	rectCount = canvas->getObjectCount();

	//sizer->Add(buttonPanel, 0, wxEXPAND | wxALL, 0);
	//sizer->Add(canvas, 1, wxEXPAND | wxALL, 0);

	//this->SetSizerAndFit(sizer);

	CreateStatusBar(1);
	SetStatusText("Ready", 0);
	
	
}


void MyFrame::OnAddButtonClick(wxCommandEvent& event)
{
	std::uniform_int_distribution<> sizeDistrib (this->FromDIP(50), this->FromDIP(100));
	std::uniform_int_distribution<> xDistrib(0, canvas->GetSize().GetWidth());
	std::uniform_int_distribution<> yDistrib(0, canvas->GetSize().GetHeight());
	std::uniform_real_distribution<> angleDistrib(0.0, M_PI * 2.0);

	std::uniform_int_distribution<> colorDistrib(0, 0xFFFFFF);

	rectCount++;
	canvas->addNote(sizeDistrib(randomGen), sizeDistrib(randomGen), xDistrib(randomGen), yDistrib(randomGen),
		wxColor(colorDistrib(randomGen)));
}

void MyFrame::OnRemoveButtonClick(wxCommandEvent& event)
{
	canvas->removeTopNote();
}

//double click to add a note, currently breaks double click to remove
//need to find a way to detect when the mouse is hovering over an existing note and disable this function when true
void MyFrame::OnMouseEvent(wxMouseEvent& evt)
{
	
	std::uniform_int_distribution<> sizeDistrib(this->FromDIP(50), this->FromDIP(100));
	std::uniform_real_distribution<> angleDistrib(0.0, M_PI * 2.0);

	std::uniform_int_distribution<> colorDistrib(0, 0xFFFFFF);

	wxPoint mousePos = evt.GetPosition();

	rectCount++;
	canvas->addNote(sizeDistrib(randomGen), sizeDistrib(randomGen), mousePos.x, mousePos.y,
		wxColor(colorDistrib(randomGen)));
	
}

void MyFrame::OnNoteAdded(wxCommandEvent& event)
{
	SetStatusText("Note named " + event.GetString() + " added!", 0);
}

void MyFrame::OnNoteRemoved(wxCommandEvent& event)
{
	SetStatusText("Note named " + event.GetString() + " REMOVED!", 0);
}

wxPanel* MyFrame::createButtonPanel(wxWindow* parent)
{
	wxPanel* panel = new wxPanel(parent);
	wxButton* addNoteButton = new wxButton(panel, wxID_ANY, "Add Note");
	wxButton* removeLastButton = new wxButton(panel, wxID_ANY, "Remove Top Note");

	wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(addNoteButton, 0, wxEXPAND | wxALL, 3);
	sizer->Add(removeLastButton, 0, wxEXPAND | wxALL, 3);

	panel->SetSizer(sizer);

	addNoteButton->Bind(wxEVT_BUTTON, &MyFrame::OnAddButtonClick, this);
	removeLastButton->Bind(wxEVT_BUTTON, &MyFrame::OnRemoveButtonClick, this);

	return panel;
}

/*
void MIDIPane::render(wxDC& dc)
{
	dc.DrawText(wxT("Testing"), 40, 60);

	// draw a circle
	dc.SetBrush(*wxGREEN_BRUSH); // green filling
	dc.SetPen(wxPen(wxColor(255, 0, 0), 5)); // 5-pixels-thick red outline
	dc.DrawCircle(wxPoint(200, 100), 25 /* radius /);

	// draw a rectangle
	dc.SetBrush(*wxBLUE_BRUSH); // blue filling
	dc.SetPen(wxPen(wxColor(255, 175, 175), 10)); // 10-pixels-thick pink outline
	dc.DrawRectangle(300, 100, 400, 200);

	// draw a line
	dc.SetPen(wxPen(wxColor(0, 0, 0), 3)); // black line, 3 pixels thick
	dc.DrawLine(300, 100, 700, 300); // draw line across the rectangle
	/*
	MidiFile midi;
	wxTimer timer;

	midi.ParseFile("battle-theme.mid");

	size_t nCurrentNote[16]{};

	double dSongTime = 0.0;
	double dRunTime = 0.0;
	uint32_t nMidiClock = 0;

	float nTrackOffset = 1000.0f;


	int screenW, screenH;
	GetClientSize(&screenW, &screenH);

	uint32_t nTimePerColumn = 50;
	uint32_t nNoteHeight = 2;
	uint32_t nOffsetY = 0;

	dc.SetFont(wxFontInfo(8).Family(wxFONTFAMILY_TELETYPE));

	for (auto& track : midi.vecTracks)
	{
		if (track.vecNotes.empty())
		{
			std::cout << "Empty Track" << std::endl;
			continue;
		}
		uint32_t nNoteRange = track.nMaxNote - track.nMinNote;
		uint32_t trackHeight = (nNoteRange + 1) * nNoteHeight;

		// Track background
		dc.SetBrush(*wxLIGHT_GREY_BRUSH);
		dc.SetPen(*wxTRANSPARENT_PEN);
		dc.DrawRectangle(0, nOffsetY, screenW, trackHeight);

		// Track name
		dc.SetTextForeground(*wxBLACK);
		dc.DrawText(track.sName, 4, nOffsetY + 2);

		// Notes
		dc.SetBrush(*wxWHITE_BRUSH);
		for (auto& note : track.vecNotes)
		{
			int x = (note.nStartTime - nTrackOffset) / nTimePerColumn;
			int y = (nNoteRange - (note.nKey - track.nMinNote)) * nNoteHeight
				+ nOffsetY;

			int w = note.nDuration / nTimePerColumn;
			int h = nNoteHeight;

			dc.DrawRectangle(x, y, w, h);
		}

		nOffsetY += trackHeight + 4;

	}
	*/
//}


/*
class olcMIDIViewer : public olc::PixelGameEngine
{
public:
	olcMIDIViewer()
	{
		sAppName = "MIDI File Viewer";
	}


	MidiFile midi;

	//HMIDIOUT hInstrument;
	size_t nCurrentNote[16]{ 0 };

	double dSongTime = 0.0;
	double dRunTime = 0.0;
	uint32_t nMidiClock = 0;


public:
	bool OnUserCreate() override
	{

		midi.ParseFile("battle-theme.mid");

		/
		int nMidiDevices = midiOutGetNumDevs();
		if (nMidiDevices > 0)
		{
			if (midiOutOpen(&hInstrument, 2, NULL, 0, NULL) == MMSYSERR_NOERROR)
			{
				std::cout << "Opened midi" << std::endl;
				change to push
			}
		}
		/


		return true;
	}

	float nTrackOffset = 1000;

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);
		uint32_t nTimePerColumn = 50;
		uint32_t nNoteHeight = 2;
		uint32_t nOffsetY = 0;

		if (GetKey(olc::Key::LEFT).bHeld) nTrackOffset -= 10000.0f * fElapsedTime;
		if (GetKey(olc::Key::RIGHT).bHeld) nTrackOffset += 10000.0f * fElapsedTime;


		for (auto& track : midi.vecTracks)
		{
			if (!track.vecNotes.empty())
			{
				uint32_t nNoteRange = track.nMaxNote - track.nMinNote;

				FillRect(0, nOffsetY, ScreenWidth(), (nNoteRange + 1) * nNoteHeight, olc::DARK_GREY);
				DrawString(1, nOffsetY + 1, track.sName);

				for (auto& note : track.vecNotes)
				{
					FillRect((note.nStartTime - nTrackOffset) / nTimePerColumn, (nNoteRange - (note.nKey - track.nMinNote)) * nNoteHeight + nOffsetY, note.nDuration / nTimePerColumn, nNoteHeight, olc::WHITE);
				}

				nOffsetY += (nNoteRange + 1) * nNoteHeight + 4;
			}
		}

		return true;
	}


};
*/
/*
int main()
{
	olcMIDIViewer demo;
	if (demo.Construct(1280, 960, 1, 1))
		demo.Start();
	return 0;
}
*/