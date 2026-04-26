
#include "helpDialog.h"

HelpDialog::HelpDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name) : wxDialog(parent, id, title, pos, size, style, name)
{
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	windowSize = size.x/1.5;

	helpSizer = new wxBoxSizer(wxVERTICAL);

	text = new wxStaticText(this, wxID_ANY, "Welcome to the tutorial! Here you'll learn how to use this DAW.");
	
	nextButton = new wxButton(this, wxID_ANY, "Next", wxPoint(FromDIP(200), FromDIP(150)) );
	helpSizer->Add(text, 1, wxALIGN_TOP, 10);
	helpSizer->AddSpacer(30);
	helpSizer->Add(nextButton, 1, wxALIGN_CENTER, 10);
	nextButton->Bind(wxEVT_BUTTON, &HelpDialog::OnButtonPress, this);

	this->SetSizerAndFit(helpSizer);
}

void HelpDialog::OnButtonPress(wxCommandEvent& event)
{
	
	tutorialPage++;
	
	switch (tutorialPage)
	{
	case 1:
		text->SetLabel("To get started, you can open a MIDI file by clicking on \"File\", then \"Open\", then selecting a file from your computer. ");
		
		break;
	case 2:
		text->SetLabel("Now, you will see each track in the file on the right, each one of those black bars is a track.");
		
		break;
	case 3:
		text->SetLabel("The white boxes inside those tracks are the MIDI notes, that's what you will be editing.");
		
		break;
	case 4:
		text->SetLabel("To open a track for editing, simply double click on the track you wish to edit.");
		
		break;
	case 5:
		text->SetLabel("You're now in the editor! You'll see the notes on the grid, and you can use the arrow keys to move to different parts of the track.");
		
		break;
	case 6:
		text->SetLabel("To add a note, double click anywhere on the grid. To remove a note, right click on the note you want to remove.");
		
		break;
	case 7:
		text->SetLabel("You can move a note to a new location by clicking and dragging the note.");
		
		break;
	case 8:
		text->SetLabel("To change a note's duration, simply click and drag while holding the ALT key on your keyboard.");
		
		break;
	case 9:
		text->SetLabel("Once you are done, simply close the window and your changes will be saved to the main window.");
		
		break;
	case 10:
		text->SetLabel("While the main window is open, clicking on the \"Play\" button will start playing the MIDI file. (Note: Tracks can't be opened for editing and \"File\" menu items will be disabled while the MIDI file is playing.)");
		
		break;
	case 11:
		text->SetLabel("While the MIDI file is playing, clicking on the \"Stop\" button will stop the MIDI file audio.");
		
		break;
	case 12:
		text->SetLabel("To save your work, click on \"File\", then \"Save\" or \"SaveAs\".");
		
		break;
	case 13:
		text->SetLabel("To create a file from scratch, click on \"File\", then \"New\".");
		break;
	case 14:
		text->SetLabel("You'll then enter the desired tempo, then select the instrument, then enter the time signature.");
		break;
	case 15:
		text->SetLabel("After that, enter the name of the file in the file manager window, and click \"Open\" once you've decided where it should be created.");
		break;
	case 16:
		text->SetLabel("And that's everything! You're now ready to write music!");
		nextButton->SetLabel("Close");
		break;
	case 17:
		this->Close();
	}
	text->Wrap(windowSize);
	
}