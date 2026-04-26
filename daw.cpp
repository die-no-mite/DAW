

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



<<<<<<< Updated upstream
=======
	float ScaleYCoord(float y, float min, float max);
	int maxX = 0;
	void UpdateMidiTrack(int trackNumber);


private:
	void process(Alg_seq_ptr seq, double tempo);

	wxPanel* BuildTrackInfoPanel(wxWindow* parent, int trackNumber);
	MidiFrame* BuildTrackPanel(wxWindow* parent, int trackNumber);
	void DrawMidiTracks();
	void Setup();
	void PlayMIDIFile(wxCommandEvent &event);

	wxSplitterWindow* ResetSplitter();

	Alg_seq_ptr seq;
	MidiPlayer *midiPlayer = new MidiPlayer;

	void BuildMenuBar();
	void OnOpen(wxCommandEvent& event);
	void OnSaveAs(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnHelp(wxCommandEvent& event);
	void OnNew(wxCommandEvent& event);
	void OnQuit(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	wxTextCtrl* m_textCtrl;

	void OnDoubleClick(wxMouseEvent& event);

	wxPanel* trackInfoPanel;
	EditorFrame* editorWindow;

	wxSplitterWindow* splitter;
	wxBoxSizer* mainSizer;

	std::string ResolveMidiPath(std::string midiPath) const;

	TrackFrame* currentTrackFrame;
	TrackManager* trackList = new TrackManager();
	TrackManager* trackInfoList = new TrackManager();

	std::vector<int> trackIDs;

	MidiFrame* canvas;
	MidiFile* midi = new MidiFile;

	bool reset = false;
	bool isFileOpen = false;
	bool isPlaying = false;
	bool saved = true;

	wxButton* playButton;
	int beginingEmptyTracks;

	std::string pathName = "";

	const std::string lightBackground = "#f4f3f3";
	const std::string darkBackground = "#2c2828";

	void OnCanvasResize(wxSizeEvent& event);
};
>>>>>>> Stashed changes

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


<<<<<<< Updated upstream
=======
void MyFrame::BuildMenuBar()
{
	wxMenuBar* menuBar = new wxMenuBar;
	wxMenu* fileMenu = new wxMenu;

	fileMenu->Append(wxID_NEW);
	Bind(wxEVT_MENU, &MyFrame::OnNew, this, wxID_NEW);
	fileMenu->Append(wxID_OPEN);
	Bind(wxEVT_MENU, &MyFrame::OnOpen, this, wxID_OPEN);
	fileMenu->Append(wxID_SAVE);
	Bind(wxEVT_MENU, &MyFrame::OnSave, this, wxID_SAVE);
	fileMenu->Append(wxID_SAVEAS);
	Bind(wxEVT_MENU, &MyFrame::OnSaveAs, this, wxID_SAVEAS);
	fileMenu->Append(wxID_EXIT);
	Bind(wxEVT_MENU, &MyFrame::OnQuit, this, wxID_EXIT);

	menuBar->Append(fileMenu, "&File");

	auto helpMenu = new wxMenu;
	helpMenu->Append(wxID_HELP);
	Bind(wxEVT_MENU, &MyFrame::OnHelp, this, wxID_HELP);

	menuBar->Append(helpMenu, "&Help");

	

	SetMenuBar(menuBar);
}

void MyFrame::OnClose(wxCloseEvent& event)
{
	if (event.CanVeto())
	{
		if (!isPlaying)
		{
			if (!saved)
			{
				auto quitSaveDialog = NewFileDialog(this, wxID_ANY, "Save?", wxDefaultPosition, wxSize(FromDIP(500), FromDIP(250)), wxDEFAULT_DIALOG_STYLE, "Save?", 3);
				quitSaveDialog.ShowModal();

				if (!quitSaveDialog.GetQuit())
					event.Veto();
				else
					Destroy();
			}
			else
				Destroy();
		}
		else
			event.Veto();
	}
	else
		Destroy();
}

void MyFrame::OnQuit(wxCommandEvent& event)
{
	this->Close();
}

void MyFrame::OnHelp(wxCommandEvent& event)
{
	auto helpDialog = new HelpDialog(this, wxID_ANY, "Tutorial", wxDefaultPosition, wxSize(FromDIP(500), FromDIP(250)), wxDEFAULT_DIALOG_STYLE, "Tutorial");
	helpDialog->ShowModal();
}

void MyFrame::OnNew(wxCommandEvent& event)
{
	if (!isPlaying)
	{
		bool earlyExit = false;
		int tempo = 0;
		int instrumentCode = 0;
		wxPoint timeSig(0, 0);

		auto tempoDialog = new NewFileDialog(this, wxID_ANY, "Set Tempo", wxDefaultPosition, wxSize(FromDIP(500), FromDIP(250)), wxDEFAULT_DIALOG_STYLE, "Set Tempo", 0);
		tempoDialog->ShowModal();
		if (!tempoDialog->GetEarlyExit())
		{
			tempo = tempoDialog->GetTempo();
			auto instrumentDialog = new NewFileDialog(this, wxID_ANY, "Set Instrument", wxDefaultPosition, wxSize(FromDIP(500), FromDIP(250)), wxDEFAULT_DIALOG_STYLE, "Set Instrument", 1);
			instrumentDialog->ShowModal();

			if (!instrumentDialog->GetEarlyExit())
			{
				instrumentCode = instrumentDialog->GetInstrument();
				auto timeSigDialog = new NewFileDialog(this, wxID_ANY, "Set Time Signature", wxDefaultPosition, wxSize(FromDIP(500), FromDIP(250)), wxDEFAULT_DIALOG_STYLE, "Set Time Signature", 2);
				timeSigDialog->ShowModal();

				if (!timeSigDialog->GetEarlyExit())
					timeSig = timeSigDialog->GetTimeSignature();
				else
					earlyExit = true;
			}
			else
				earlyExit = true;

		}
		else
			earlyExit = true;
		wxFileDialog fileDialog(this, _("New"), "", "",
			"MIDI files (*.mid)|*.mid|All files (*.*)|*.*",
			wxFD_OPEN);

		if (!earlyExit && fileDialog.ShowModal() == wxID_OK)
		{
			//get the path of the selected file
			wxString fileLocationWX = fileDialog.GetPath();
			//convert to string from wxString

			if (isFileOpen) // only sets reset flag if there's already a file open
				reset = true;

			//destroys panels contained within the list and deletes empty cells
			trackList->DetroyList();
			trackInfoList->DetroyList();


			midi = new MidiFile();
			seq = new Alg_seq;
			seq->get_time_map()->insert_tempo(tempo, 0);
			seq->add_track(0);
			seq->set_time_sig(0, timeSig.x, timeSig.y);
			Alg_update_ptr update;
			if (instrumentCode != 129)
			{
				update = seq->create_update(0, 0, 4);
				update->set_int64_value("programi", instrumentCode);
				seq->track(0)->add(update);
				auto initialNote = seq->create_note(0, 0, 57, 57, 127, 20);
				seq->track(0)->add(initialNote);
			}
			else
			{
				update = seq->create_update(0, 9, 9);
				update->set_real_value("control7r", 0.8189);
				seq->track(0)->add(update);

				update = seq->create_update(0, 9, 9);
				update->set_real_value("control10r", 0.5039);
				seq->track(0)->add(update);

				update = seq->create_update(0, 9, 9);
				update->set_real_value("control0r", 0);
				seq->track(0)->add(update);

				update = seq->create_update(0, 9, 9);
				update->set_real_value("control32r", 0.0157);
				seq->track(0)->add(update);

				update = seq->create_update(0, 9, 4);
				update->set_int64_value("programi", 0);
				seq->track(0)->add(update);

				auto initialNote = seq->create_note(0, 9, 35, 35, 127, 20);
				seq->track(0)->add(initialNote);
			}

			std::ofstream file;
			file.open("output.txt");
			seq->write(file, false);
			file.close();

			auto realFileName = (const_cast<char*>((const char*)fileLocationWX.mb_str()));

			seq->smf_write(realFileName, 480);
			seq->convert_to_beats();

			midi->ParseFile(realFileName);

			for (int i = 0; i < midi->vecTracks.size(); i++)
			{
				for (auto event : midi->vecTracks[i].vecEvents)
				{
					if (event.event == MidiEvent::Type::Program)
					{
						if (event.nProgram == 0 || event.nChannel == 9)
							midi->vecTracks[i].isPercusion = true;

					}
				}
			}

			isFileOpen = true;

			Setup();

			SetStatusText("Created new file.");
		}
	}
}

void MyFrame::OnOpen(wxCommandEvent& event)
{
	event.Skip();
	if (!isPlaying)
	{
		wxString wildcard = wxT("MIDI files (*.mid)|*.mid|All files (*.*)|*.*");

		wxFileDialog openFileDialog(this, _("Open File"), "", "",
			"MIDI files (*.mid)|*.mid|All files (*.*)|*.*",
			wxFD_OPEN | wxFD_FILE_MUST_EXIST);

		//show dialog and check if user clicked "OK"
		if (openFileDialog.ShowModal() == wxID_OK)
		{
			//get the path of the selected file
			wxString fileLocationWX = openFileDialog.GetPath();
			//convert to string from wxString
			std::string fileLocation = std::string(fileLocationWX.mb_str(wxConvUTF8));


			if (isFileOpen) // only sets reset flag if there's already a file open
				reset = true;

			//destroys panels contained within the list and deletes empty cells
			trackList->DetroyList();
			trackInfoList->DetroyList();

			pathName = ResolveMidiPath(fileLocation);
			SetStatusText("Opened: " + pathName);
			midi = new MidiFile();
			std::ifstream ifile(pathName, std::ios::binary | std::ios::in);
			seq = new Alg_seq(ifile, true);
			midi->ParseFile(fileLocation);
			ifile.close();


			std::ofstream file;
			file.open("output.txt");
			seq->write(file, false);
			file.close();

			beginingEmptyTracks = 0;
			//removes any empty tracks and counts how many are removed before the first nonempty track
			for (auto track : midi->vecTracks)
			{
				if (track.vecNotes.empty())
				{
					beginingEmptyTracks++;
				}
				else
					break;
			}
			midi->trackIndexOffset = beginingEmptyTracks;
			midi->vecTracks.erase(std::remove_if(midi->vecTracks.begin(), midi->vecTracks.end(),
				[](const MidiTrack& t) {return t.vecNotes.empty(); }), midi->vecTracks.end());

			for (int i = 0; i < midi->vecTracks.size(); i++)
			{
				for (auto event : midi->vecTracks[i].vecEvents)
				{
					if (event.event == MidiEvent::Type::Program)
					{
						if (event.nProgram == 0 || event.nChannel == 9)
							midi->vecTracks[i].isPercusion = true;
						
					}
				}
			}

			file.open("output.txt");
			file << midi->vecTracks[0].isPercusion << std::endl;
			file.close();

			isFileOpen = true;

			Setup();
		}
		else
		{
			//user cancelled the dialog
			SetStatusText("File selection cancelled.");
		}
	}
	
}



void MyFrame::OnSaveAs(wxCommandEvent& event)
{
	event.Skip();
	if (!isPlaying)
	{

		wxFileDialog openFileDialog(this, _("Save As"), "", "",
			"MIDI files (*.mid)|*.mid|All files (*.*)|*.*",
			wxFD_SAVE);

		if (openFileDialog.ShowModal() == wxID_OK)
		{
			wxString filePath = openFileDialog.GetPath();


			auto realFilePath = (const_cast<char*>((const char*)filePath.mb_str()));

			//writes to the file
			seq->smf_write(realFilePath, midi->nDivision);
			pathName = realFilePath;

			trackList->DetroyList();
			trackInfoList->DetroyList();

			//opens the new file that was created/saved to
			std::ifstream ifile(pathName, std::ios::binary | std::ios::in);
			seq = new Alg_seq(ifile, true);
			midi->ParseFile(pathName);
			ifile.close();

			//removes any empty tracks
			midi->vecTracks.erase(std::remove_if(midi->vecTracks.begin(), midi->vecTracks.end(),
				[](const MidiTrack& t) {return t.vecNotes.empty(); }), midi->vecTracks.end());

			isFileOpen = true;
			reset = true;
			saved = true;

			Setup();

			SetStatusText("Saved: " + filePath);
		}
	}
}

void MyFrame::OnSave(wxCommandEvent& event)
{
	event.Skip();
	if (!isPlaying)
	{

		if (pathName != "")
		{
			wxString filePath = pathName;
			auto realFilePath = (const_cast<char*>((const char*)filePath.mb_str()));

			seq->smf_write(realFilePath, midi->nDivision);

			saved = true;
			SetStatusText("Saved: " + filePath);
		}
	}
}

>>>>>>> Stashed changes

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
<<<<<<< Updated upstream
	NULL;
=======

	auto* trackPanel = new MidiFrame(parent, wxID_ANY, wxDefaultPosition, wxSize(200, 100));

	trackPanel->SetScrollRate(FromDIP(10), FromDIP(10));
	trackPanel->SetVirtualSize(200, 100);
	trackPanel->SetBackgroundColour(wxColor(70, 70, 70));


	wxSizer* trackSizer = new wxBoxSizer(wxVERTICAL);


	auto text = new wxStaticText(trackPanel, wxID_ANY, "Tracks:");
	text->SetForegroundColour(wxColor(255, 255, 255));
	trackSizer->Add(text, 0, wxALL, FromDIP(5));


	if (trackNumber == 1) //sets up track if there's only one track
	{
		trackList->setIndex(0);
		trackList->addTrack(new TrackFrame(trackPanel, wxID_ANY, wxDefaultPosition, wxSize(1000, 100)));
		trackList->getTrackFrame()->SetBackgroundColour(wxColor(0, 0, 0));
		trackIDs.push_back(trackList->getTrackFrame()->GetId());
		trackSizer->Add(trackList->getTrackFrame(), 0, wxEXPAND | wxALL, 5);
		trackList->getTrackFrame()->Bind(wxEVT_LEFT_DCLICK, &MyFrame::OnDoubleClick, this);

	}
	else
	{

		//fills vector with midiframes per track and adds them to the sizer
		for (int i = 0; i < trackNumber; i++)
		{
			text = new wxStaticText(trackPanel, wxID_ANY, midi->vecTracks[i].sName);
			text->SetForegroundColour(wxColor(255, 255, 255));
			trackSizer->Add(text, 0, wxTOP | wxALL);
			trackList->setIndex(i);
			trackList->addTrack(new TrackFrame(trackPanel, wxID_ANY, wxDefaultPosition, wxSize(1000, 100)));
			trackList->getTrackFrame()->SetBackgroundColour(wxColor(0, 0, 0));
			trackIDs.push_back(trackList->getTrackFrame()->GetId());
			if (i == 0)
				trackSizer->Add(trackList->getTrackFrame(), 0, wxEXPAND | wxALL | wxRIGHT | wxTOP, 5); //first track only has a boarder on the left right and top
			else if (i == trackNumber - 1)
				trackSizer->Add(trackList->getTrackFrame(), 0, wxEXPAND | wxALL | wxRIGHT | wxBOTTOM, 5); //last track only has a boarder on the left right and bottom
			else
				trackSizer->Add(trackList->getTrackFrame(), 0, wxEXPAND | wxALL | wxRIGHT, 5); //middle tracks only have a boarder on the left and right


			trackList->getTrackFrame()->Bind(wxEVT_LEFT_DCLICK, &MyFrame::OnDoubleClick, this);

		}

	}



	trackPanel->SetSizerAndFit(trackSizer);

	return trackPanel;
>>>>>>> Stashed changes
}


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(nullptr, wxID_ANY, title, pos, size)
{
<<<<<<< Updated upstream
	
	wxSplitterWindow *splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);
=======
	splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

	mainSizer = new wxBoxSizer(wxHORIZONTAL);

	std::string pathName = ResolveMidiPath("battle-theme.mid");

	wxString worked;


	worked = pathName;

	Bind(wxEVT_CLOSE_WINDOW, &MyFrame::OnClose, this);

	BuildMenuBar();


	CreateStatusBar(1);
	SetStatusText("", 0);

}

wxSplitterWindow* MyFrame::ResetSplitter()
{
	return new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);
}


void MyFrame::PlayMIDIFile(wxCommandEvent& event)
{
	if (!isPlaying)
	{
		playButton->SetLabel("Stop");
		midiPlayer->seq_play(seq);
		isPlaying = true;
	}
	else
	{
		playButton->SetLabel("Play");
		midiPlayer->pauseMidi();
		isPlaying = false;
	}
}


void MyFrame::Setup()
{

	std::ofstream file;
	file.open("output.txt");
	file << midi->vecTracks[0].isPercusion << std::endl;
	file.close();
	mainSizer->Clear(true); // clears the main sizer, deleting it's contents in the process


	//gets the number of tracks from the midi file and creates a vector (dynamic array) of panels of the size of the track number
	int trackNumber = midi->getTrackNum();


	if (reset) // if a reset is needed, the trackIDs vector is cleared and the splitter is reset
	{

		trackIDs.erase(trackIDs.begin(), trackIDs.end());

		splitter = ResetSplitter();

		reset = false;
	}
>>>>>>> Stashed changes

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

<<<<<<< Updated upstream
		return true;
=======
		std::ofstream file;
		file.open("output.txt");
		file << midi->vecTracks[0].isPercusion << std::endl;
		file.close();


		editorWindow = new EditorFrame(this, wxID_ANY, "Editor", wxDefaultPosition, wxSize(FromDIP(1000), FromDIP(500)), wxDEFAULT_DIALOG_STYLE, "Editor", midi, seq);

		editorWindow->Bind(EVT_UPDATE_TRACK, [&](TrackUpdateEvent& event) {
			int trackToUpdate = event.GetTrackNumber();
			midi->vecTracks[trackToUpdate].nMinNote = editorWindow->GetNewMinMax().x;
			midi->vecTracks[trackToUpdate].nMaxNote = editorWindow->GetNewMinMax().y;
			UpdateMidiTrack(trackToUpdate);
			event.Skip();
			});
		editorWindow->ShowModal();


		//std::ofstream file;
		file.open("output.txt");
		seq->write(file, false);
		file.close();
	
	}
	
}

std::string MyFrame::ResolveMidiPath(std::string sMidiPath) const
{
	namespace fs = std::filesystem;

	std::vector<fs::path> candidates = {
		fs::path(sMidiPath),
		fs::path("./") / sMidiPath,
		fs::path("../") / sMidiPath,
		fs::path("../../") / sMidiPath,
		fs::path("../../../") / sMidiPath,
		fs::path("../../../../") / sMidiPath,
		fs::path("../../../../../") / sMidiPath
	};

	for (const auto& p : candidates)
	{
		if (fs::exists(p))
			return p.string();
>>>>>>> Stashed changes
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