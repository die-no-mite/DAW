

#include <fstream>
#include <array>
#include <vector>
#include <random>
#include <filesystem>

#include "MidiC.h"
#include "MidiEvent.h"
#include "MidiNote.h"
#include "MidiTrack.h"
#include "midiFrame.h"
#include "trackFrame.h"
#include "TrackManager.h"
#include "editorFrame.h"

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

	float ScaleYCoord(float y, float min, float max);

	

private:
	
	wxPanel *BuildTrackInfoPanel(wxWindow* parent, int trackNumber);
	MidiFrame* BuildTrackPanel(wxWindow* parent, int trackNumber);
	void DrawMidiTracks();
	void Setup();
	
	wxSplitterWindow* ResetSplitter();

	void BuildMenuBar();
	void OnOpen(wxCommandEvent& event);
	void OnSaveAs(wxCommandEvent& event);
	wxTextCtrl* m_textCtrl;

	void OnDoubleClick(wxMouseEvent &event);


	wxPanel* trackInfoPanel;

	wxSplitterWindow* splitter;
	wxBoxSizer* mainSizer;

	std::string ResolveMidiPath(std::string midiPath) const;

	TrackFrame* currentTrackFrame;
	TrackManager* trackList = new TrackManager();
	TrackManager* trackInfoList = new TrackManager();

	std::vector<int> trackIDs;

	MidiFrame *canvas;
	MidiFile* midi = new MidiFile;

	int rectCount = 0;
	std::mt19937 randomGen;

	bool isEditorOpen = false;
	bool reset = false;
	bool isFileOpen = false;


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



wxPanel* MyFrame::BuildTrackInfoPanel(wxWindow* parent, int trackNumber)
{
	auto trackInfoPanel = new wxScrolled<wxPanel>(parent, wxID_ANY);
	trackInfoPanel->SetScrollRate(0, FromDIP(10));

	bool isDark = wxSystemSettings::GetAppearance().IsDark();
	trackInfoPanel->SetBackgroundColour(wxColor(isDark ? darkBackground : lightBackground));

	auto mainSizer = new wxBoxSizer(wxVERTICAL);
	auto perTrackSizer = new wxBoxSizer(wxVERTICAL);

	
	
	auto text = new wxStaticText(trackInfoPanel, wxID_ANY, "Track Information:");
	auto text2 = new wxStaticText(trackInfoPanel, wxID_ANY, "");
	auto text3 = new wxStaticText(trackInfoPanel, wxID_ANY, "");
	auto text4 = new wxStaticText(trackInfoPanel, wxID_ANY, "");
	
	mainSizer->Add(text, 0, wxALL, FromDIP(5));

	

	std::string trackNumberText = "";
	for (int i = 0; i < trackNumber; i++)
	{
		trackInfoList->setIndex(i);
		trackInfoList->addTrack(new TrackFrame(trackInfoPanel, wxID_ANY, wxDefaultPosition, wxSize(200, 100)));
		trackInfoList->getTrackFrame()->SetBackgroundColour(wxColor(0, 0, 0));
		mainSizer->Add(trackInfoList->getTrackFrame(), 0, wxEXPAND | wxALL, 5);
		
		
		trackNumberText = "Track #";
		trackNumberText += std::to_string(i+1);
		perTrackSizer = new wxBoxSizer(wxVERTICAL);
		text = new wxStaticText(trackInfoList->getTrackFrame(), wxID_ANY, trackNumberText);
		text->SetForegroundColour(wxColor(255, 255, 255));
		perTrackSizer->Add(text, 0, wxALL);
		
		
		text2 = new wxStaticText(trackInfoList->getTrackFrame(), wxID_ANY, "");
		perTrackSizer->Add(text2, 0, wxALL, FromDIP(5));
		text3 = new wxStaticText(trackInfoList->getTrackFrame(), wxID_ANY, "");
		perTrackSizer->Add(text3, 0, wxALL, FromDIP(5));
		text4 = new wxStaticText(trackInfoList->getTrackFrame(), wxID_ANY, "");
		perTrackSizer->Add(text4, 0, wxALL, FromDIP(5));
		
		
		
		trackInfoList->getTrackFrame()->SetSizerAndFit(perTrackSizer);
		
		
	}

	mainSizer->AddStretchSpacer();
	
	mainSizer->AddSpacer(FromDIP(5));

	trackInfoPanel->SetSizer(mainSizer);
	

	return trackInfoPanel;
}

void MyFrame::BuildMenuBar()
{
	wxMenuBar* menuBar = new wxMenuBar;
	wxMenu* fileMenu = new wxMenu;

	fileMenu->Append(wxID_NEW);
	fileMenu->Append(wxID_OPEN);
	Bind(wxEVT_MENU, &MyFrame::OnOpen, this, wxID_OPEN);
	fileMenu->Append(wxID_SAVE);
	fileMenu->Append(wxID_SAVEAS);
	Bind(wxEVT_MENU, &MyFrame::OnSaveAs, this, wxID_SAVEAS);
	fileMenu->Append(wxID_CLOSE);
	fileMenu->Append(wxID_EXIT);

	menuBar->Append(fileMenu, "&File");

	auto editMenu = new wxMenu;
	editMenu->Append(wxID_UNDO);
	editMenu->Append(wxID_REDO);
	editMenu->AppendSeparator();
	editMenu->Append(wxID_CUT);
	editMenu->Append(wxID_COPY);
	editMenu->Append(wxID_PASTE);
	editMenu->Append(wxID_DELETE);
	editMenu->AppendSeparator();
	editMenu->Append(wxID_SELECTALL);

	menuBar->Append(editMenu, "&Edit");

	SetMenuBar(menuBar);
}

void MyFrame::OnOpen(wxCommandEvent& event)
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

		
		if(isFileOpen) // only sets reset flag if there's already a file open
			reset = true;

		//destroys panels contained within the list and deletes empty cells
		trackList->DetroyList();
		trackInfoList->DetroyList();
		
		std::string pathName = ResolveMidiPath(fileLocation);
		SetStatusText("Opened: " + pathName);
		midi = new MidiFile();
		midi->ParseFile(fileLocation);
		//fileReady = fileLocation;
		
		isFileOpen = true;

		Setup();
	}
	else
	{
		//user cancelled the dialog
		SetStatusText("File selection cancelled.");
	}
}

/*
void MyFrame::OnOpen(wxCommandEvent& event)
{
	wxFileDialog openFileDialog(this, _("Open File"), "", "",
		"MIDI files (*.mid)|*.mid|All files (*.*)|*.*",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_OK)
	{
		wxString filePath = openFileDialog.GetPath();

		LoadCustomMidi(filePath);

		//SetStatusText("Opened: " + filePath);
	}

}

bool LoadCustomMidi(const std::string& filename) {
	MidiFile* midi = new MidiFile;

	if (m_textCtrl)
	{
		m_textCtrl->LoadFile(filename);
	}

	// Replace "battle-theme.mid" with the variable filename
	std::string pathName = ResolveMidiPath(filename);

	if (!midi->ParseFile(pathName)) {
		std::cerr << "Error: Could not read file " << filename << std::endl;
		delete midi;
		return false;
	}

	SetStatusText("Opened: " + pathName);

	delete midi;
	return true;
}
*/

void MyFrame::OnSaveAs(wxCommandEvent& event)
{
	wxFileDialog openFileDialog(this, _("Open File"), "", "",
		"MIDI files (*.mid)|*.mid|All files (*.*)|*.*",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_OK)
	{
		wxString filePath = openFileDialog.GetPath();


		if (m_textCtrl)
		{
			m_textCtrl->LoadFile(filePath);
		}

		SetStatusText("Saved: " + filePath);
	}

}


wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	wxInitAllImageHandlers();

	if (!wxApp::OnInit())
		return false;

	
	MyFrame *frame = new MyFrame("DAW", wxDefaultPosition, wxDefaultSize);
	frame->Show(true);

	return true;
}


MidiFrame* MyFrame::BuildTrackPanel(wxWindow* parent, int trackNumber)
{
	
	auto* trackPanel = new MidiFrame(parent, wxID_ANY, wxDefaultPosition, wxSize(200, 100));
	
	trackPanel->SetScrollRate(FromDIP(10), FromDIP(10));
	trackPanel->SetBackgroundColour(wxColor(70, 70, 70));

	//sizers to handle both the splitter/track area panel and the track panels that sit within the track area panel

	wxSizer* trackSizer = new wxBoxSizer(wxVERTICAL);


	auto text = new wxStaticText(trackPanel, wxID_ANY, "Tracks:");
	text->SetForegroundColour(wxColor(255, 255, 255));
	trackSizer->Add(text, 0, wxALL, FromDIP(5));


	if (trackNumber == 1) //sets up track if there's only one track
	{
		trackList->setIndex(0);
		trackList->addTrack(new TrackFrame(trackPanel, 8000, wxDefaultPosition, wxSize(200, 100)));
		trackList->getTrackFrame()->SetBackgroundColour(wxColor(0, 0, 0));
		trackSizer->Add(trackList->getTrackFrame(), 0, wxEXPAND | wxALL, 5);
	}
	else
	{
		int currentID = 8000;
		//fills vector with midiframes per track and adds them to the sizer
		for (int i = 0; i < trackNumber; i++)
		{
			trackList->setIndex(i);
			trackList->addTrack(new TrackFrame(trackPanel, currentID, wxDefaultPosition, wxSize(200, 100)));
			trackList->getTrackFrame()->SetBackgroundColour(wxColor(0, 0, 0));
			trackIDs.push_back(currentID);
			if (i == 0)
				trackSizer->Add(trackList->getTrackFrame(), 0, wxEXPAND | wxALL | wxRIGHT | wxTOP, 5); //first track only has a boarder on the left right and top
			else if (i == trackNumber - 1)
				trackSizer->Add(trackList->getTrackFrame(), 0, wxEXPAND | wxALL | wxRIGHT | wxBOTTOM, 5); //last track only has a boarder on the left right and bottom
			else
				trackSizer->Add(trackList->getTrackFrame(), 0, wxEXPAND | wxALL | wxRIGHT, 5); //middle tracks only have a boarder on the left and right

			trackList->getTrackFrame()->Bind(wxEVT_LEFT_DCLICK, &MyFrame::OnDoubleClick, this);
			currentID++;
		}

	}



	trackPanel->SetSizerAndFit(trackSizer);

	return trackPanel;
}

void MyFrame::DrawMidiTracks()
{
	int timePerColumn = 50;
	int noteHeight = 2;
	int i = 0;
	int j = 0;
	float trackOffset = 0;
	float ypos = 0;
	float realMin = 1000;
	float realMax = 0;

	std::vector<float> currentYList;

	std::ofstream file;
	file.open("output.txt");

	for (auto& track : midi->vecTracks)
	{
		if (!track.vecNotes.empty())
		{
			uint32_t noteRange = track.nMaxNote - track.nMinNote;
			trackList->setIndex(i);
			file << "new track" << std::endl;

			// gets the note hieghts for each note in the track and pushes them into a vector
			for (auto& note : track.vecNotes)
			{

				ypos = (noteRange - (note.nKey - track.nMinNote)) * noteHeight;
				
				currentYList.push_back(ypos);

			}

			// searches the vector for the real minimum and real maximum note height
			for (int i = 0; i < currentYList.size(); i++)
			{
				if (currentYList[i] != 0)
				{
					if (currentYList[i] < realMin)
						realMin = currentYList[i];

					if (currentYList[i] > realMax)
						realMax = currentYList[i];
				}
			}

			j = 0;
			// scales the notes to fit within the track panel and draws them in the correct panel
			
			for (auto& note : track.vecNotes)
			{
				ypos = ScaleYCoord(currentYList[j], realMin, realMax);
				file << currentYList[j] << " " << ypos << std::endl;
				trackList->getTrackFrame()->addNote(note.nDuration / timePerColumn, noteHeight, (note.nStartTime - trackOffset) / timePerColumn, ypos);
				//trackList->getTrackFrame()->addNote(note.nDuration / timePerColumn, noteHeight, (note.nStartTime - trackOffset) / timePerColumn, (noteRange - (note.nKey - track.nMinNote)) * noteHeight);
				j++;
			}

			//resets the min and max values
			realMin = 1000;
			realMax = 0;

		}
		i++;
	}
}


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(nullptr, wxID_ANY, title, pos, size)
{
	splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

	mainSizer = new wxBoxSizer(wxHORIZONTAL);
	
	std::string pathName = ResolveMidiPath("battle-theme.mid");
	//bool test = midi->ParseFile(pathName);

	wxString worked;

	
	worked = pathName;
	


	BuildMenuBar();
	
	
	CreateStatusBar(1);
	SetStatusText(worked, 0);
	
	
}

wxSplitterWindow* MyFrame::ResetSplitter()
{
	return new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);
}

void MyFrame::Setup()
{
	
	mainSizer->Clear(true); // clears the main sizer, deleting it's contents in the process

	
	

	//gets the number of tracks from the midi file and creates a vector (dynamic array) of panels of the size of the track number
	int trackNumber = midi->getTrackNum();
	

	if (reset) // if a reset is needed, the trackIDs vector is cleared and the splitter is reset
	{
		
		trackIDs.erase(trackIDs.begin(), trackIDs.end());

		splitter = ResetSplitter();

		reset = false;
	}
	
	splitter->SetMinimumPaneSize(FromDIP(150));
	
	trackInfoPanel = BuildTrackInfoPanel(splitter, trackNumber);
	canvas = BuildTrackPanel(splitter, trackNumber);
	
	mainSizer->Add(splitter, 1, wxEXPAND, 0);


	splitter->SplitVertically(trackInfoPanel, canvas);
	splitter->SetSashPosition(FromDIP(220));

	this->SetSize(FromDIP(800), FromDIP(500));
	this->SetMinSize({ FromDIP(400), FromDIP(200) });
	this->SetSizerAndFit(mainSizer);


	DrawMidiTracks();
}


float MyFrame::ScaleYCoord(float y, float min, float max)
{
	float scaledY;
	float range = max - min;
	float ystd = (y - min) / range;

	if (range > 100)
		scaledY = (ystd * (90.0 - 10.0) + 10.0);
	else if (range > 80)
		scaledY = (ystd * (80.0 - 20.0) + 20.0);
	else if (range > 60)
		scaledY = (ystd * (70.0 - 30.0) + 30.0);
	else
		scaledY = (ystd * (65.0 - 35.0) + 35.0);
	return scaledY;
}




//double click to open a track in the editor
void MyFrame::OnDoubleClick(wxMouseEvent& evt)
{
	for (int i = 0; i < trackIDs.size(); i++)
	{
		if (evt.GetId() == trackIDs[i]) // determines which track was opened and stores its index in the midi object
		{
			midi->currentTrack = i;
		}
	}

	EditorFrame* editorWindow = new EditorFrame(this, wxID_ANY, "Editor", wxDefaultPosition, wxSize(FromDIP(1000), FromDIP(500)), wxDEFAULT_DIALOG_STYLE, "Editor", midi);

	editorWindow->ShowModal();
	
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
	}

	return sMidiPath;
}
