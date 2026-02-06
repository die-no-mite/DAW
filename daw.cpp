#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <fstream>
#include <array>
#include "MidiC.h"
#include "MidiEvent.h"
#include "MidiNote.h"
#include "MidiTrack.h"
#include <wx/wx.h>

//#pragma comment(lib, "winmm.lib")


class MyApp : public wxApp
{
public:
	bool OnInit() override;
};

// This defines the equivalent of main() for the current platform.

wxIMPLEMENT_APP(MyApp);

class MyFrame : public wxFrame
{
public:
	MyFrame();

private:
	void OnHello(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
};

bool MyApp::OnInit()
{
	MyFrame *frame = new MyFrame();
	frame->Show();
	return true;
}

enum
{
	ID_Hello = 1
};

MyFrame::MyFrame()
		: wxFrame(nullptr, wxID_ANY, "Hello World")
{
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_Hello, "&Hello...\tCtrl+H",
					 "Help string shown in status bar for this menu item");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");

	SetMenuBar(menuBar);

	CreateStatusBar();
	SetStatusText("Welcome to wxWidgets!");
	Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
	Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
	Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}

void MyFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox("This is a wxWidgets Hello World example",
				 "About Hello World", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event)
{
	wxLogMessage("Hello world from wxWidgets!");
}


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

		/*
		int nMidiDevices = midiOutGetNumDevs();
		if (nMidiDevices > 0)
		{
			if (midiOutOpen(&hInstrument, 2, NULL, 0, NULL) == MMSYSERR_NOERROR)
			{
				std::cout << "Opened midi" << std::endl;
				change to push
			}
		}
		*/


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
/*
int main()
{
	olcMIDIViewer demo;
	if (demo.Construct(1280, 960, 1, 1))
		demo.Start();
	return 0;
}
*/