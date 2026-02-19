

#include <fstream>
#include <array>
#include "MidiC.h"
#include "MidiEvent.h"
#include "MidiNote.h"
#include "MidiTrack.h"


#include <wx/wx.h>
#include <wx/timer.h>

//#pragma comment(lib, "winmm.lib")




class MyApp : public wxApp
{

	bool OnInit();

	wxFrame* frame;
public:

};

// This defines the equivalent of main() for the current platform.


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

enum
{
	Minimal_Quit = wxID_EXIT,
	Minimal_About = wxID_ABOUT
};



wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	MidiFile midi;

	size_t nCurrentNote[16]{ 0 };

	double dSongTime = 0.0;
	double dRunTime = 0.0;
	uint32_t nMidiClock = 0;





	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

	frame = new wxFrame((wxFrame*)NULL, -1, wxT("DAW"), wxPoint(50, 50), wxSize(800, 600));


	frame->SetSizer(sizer);
	frame->SetAutoLayout(true);
	frame->Show(true);

	return true;
}

wxBEGIN_EVENT_TABLE(MIDIPane, wxPanel)
//EVT_MENU(Minimal_Quit, MIDIPane::OnQuit)
//EVT_MENU(Minimal_About, MIDIPane::OnAbout)
EVT_KEY_DOWN(MIDIPane::OnKeyDown)
EVT_TIMER(wxID_ANY, MIDIPane::OnTimer)
EVT_PAINT(MIDIPane::paintEvent)
wxEND_EVENT_TABLE()

MIDIPane::MIDIPane(wxFrame* parent) :
	wxPanel(parent)
{

}

void MIDIPane::paintEvent(wxPaintEvent& evt)
{
	wxPaintDC dc(this);
	render(dc);
}

void MIDIPane::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void MIDIPane::render(wxDC& dc)
{
	dc.DrawText(wxT("Testing"), 40, 60);

	// draw a circle
	dc.SetBrush(*wxGREEN_BRUSH); // green filling
	dc.SetPen(wxPen(wxColor(255, 0, 0), 5)); // 5-pixels-thick red outline
	dc.DrawCircle(wxPoint(200, 100), 25 /* radius */);

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
}


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