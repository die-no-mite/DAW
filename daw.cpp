

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
#include "PenSizePane.h"


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

	void SelectColorPane(ColorPane* pane);

	void SelectPenPane(PenSizePane* pane);

private:
	void SetupPenPanes(wxWindow* parent, wxSizer* sizer);
	wxPanel *BuildTrackInfoPanel(wxWindow* parent);

	void OnAddButtonClick(wxCommandEvent &event);
	void OnRemoveButtonClick(wxCommandEvent &event);
	void OnMouseEvent(wxMouseEvent &event);

	void OnNoteAdded(wxCommandEvent& event);
	void OnNoteRemoved(wxCommandEvent& event);

	wxPanel* createButtonPanel(wxWindow* parent);

	std::vector<ColorPane*> colorPanes{};
	std::vector<PenSizePane*> penPanes{};

	MidiFrame *canvas;

	int rectCount = 0;
	std::mt19937 randomGen;

	const std::vector<std::string> niceColors = { "#000000", "#ffffff", "#fd7f6f",
												 "#7eb0d5", "#b2e061", "#bd7ebe",
												 "#ffb55a", "#ffee65", "#beb9db",
												 "#fdcce5", "#8bd3c7" };

	const int penCount = 6;

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

void MyFrame::SetupPenPanes(wxWindow* parent, wxSizer* sizer)
{
	for (int i = 0; i < penCount; i++)
	{
		auto penPane = new PenSizePane(parent, wxID_ANY, i * FromDIP(4) + FromDIP(1));

		penPane->Bind(wxEVT_LEFT_DOWN, [this, penPane](wxMouseEvent& event)
			{ SelectPenPane(penPane); });

		penPanes.push_back(penPane);
		sizer->Add(penPane, 0, wxRIGHT | wxBOTTOM, FromDIP(5));
	}
}

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

	text = new wxStaticText(trackInfoPanel, wxID_ANY, "Pens");
	mainSizer->Add(text, 0, wxALL, FromDIP(5));

	auto penPaneSizer = new wxWrapSizer(wxHORIZONTAL);
	SetupPenPanes(trackInfoPanel, penPaneSizer);
	mainSizer->Add(penPaneSizer, 0, wxALL, FromDIP(5));

	auto button = new wxButton(trackInfoPanel, wxID_ANY, "Save As...");

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

	MyFrame *frame = new MyFrame("DAW", wxDefaultPosition, wxDefaultSize);
	frame->Show(true);

	MidiFile midi;

	size_t nCurrentNote[16]{ 0 };

	double dSongTime = 0.0;
	double dRunTime = 0.0;
	uint32_t nMidiClock = 0;


	//wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);


	return true;
}


void MyFrame::SetupInfoPanes(wxWindow* parent, wxSizer* sizer)
{
	for (const auto& color : niceColors)
	{
		auto colorPane = new ColorPane(parent, wxID_ANY, wxColour(color));

		colorPane->Bind(wxEVT_LEFT_DOWN, [this, colorPane](wxMouseEvent& event)
			{ SelectColorPane(colorPane); });

		colorPanes.push_back(colorPane);
		sizer->Add(colorPane, 0, wxRIGHT | wxBOTTOM, FromDIP(5));
	}
}


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(nullptr, wxID_ANY, title, pos, size)
{
	
	wxSplitterWindow *splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

	splitter->SetMinimumPaneSize(FromDIP(150));
	 
	auto trackInfoPanel = BuildTrackInfoPanel(splitter);
	canvas = new MidiFrame(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	

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

void MyFrame::SelectColorPane(ColorPane* pane)
{
	for (auto colorPane : colorPanes)
	{
		colorPane->selected = (colorPane == pane);
		colorPane->Refresh();
	}

	//canvas->currentColor = pane->color;
}

void MyFrame::SelectPenPane(PenSizePane* pane)
{
	for (auto penPane : penPanes)
	{
		penPane->selected = (penPane == pane);
		penPane->Refresh();
	}

	//canvas->currentWidth = pane->penWidth;
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
		wxColor(colorDistrib(randomGen)), "Note #" + std::to_string(rectCount));
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
		wxColor(colorDistrib(randomGen)), "Note #" + std::to_string(rectCount));
	
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