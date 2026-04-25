
#include "newFileDialog.h"

NewFileDialog::NewFileDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name, int prompt) : wxDialog(parent, id, title, pos, size, style, name)
{
	tempoSizer = new wxBoxSizer(wxVERTICAL);
	instrumentSizer = new wxBoxSizer(wxVERTICAL);
	timeSigSizer = new wxBoxSizer(wxVERTICAL);
	quitSizer = new wxBoxSizer(wxVERTICAL);
	switch (prompt) 
	{
	case 0:
		PromptTempo();
		break;
	case 1:
		PromptInstrument();
		break;
	case 2:
		PromptTimeSig();
		break;
	case 3:
		PromptSave();
		break;
	}
	
}

void NewFileDialog::PromptSave()
{
	text = new wxStaticText(this, wxID_ANY, "You have unsaved changes, would you like to quit anyways?");
	nextButton = new wxButton(this, wxID_ANY, "Go back");
	wxButton* quitButton = new wxButton(this, wxID_ANY, "Quit");
	quitID = quitButton->GetId();
	nextButton->Bind(wxEVT_BUTTON, &NewFileDialog::OnQuitDecision, this);
	quitButton->Bind(wxEVT_BUTTON, &NewFileDialog::OnQuitDecision, this);

	quitSizer->AddSpacer(30);
	quitSizer->Add(text, 0, wxALIGN_CENTER_HORIZONTAL);
	quitSizer->AddSpacer(30);
	quitSizer->Add(nextButton, 0, wxALIGN_CENTER_HORIZONTAL);
	quitSizer->AddSpacer(30);
	quitSizer->Add(quitButton, 0, wxALIGN_CENTER_HORIZONTAL);

	this->SetSizer(quitSizer);

}

void NewFileDialog::PromptTempo()
{
	text = new wxStaticText(this, wxID_ANY, "Enter a tempo:");
	textField = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	textField->SetEditable(true);
	textField->Bind(wxEVT_TEXT_ENTER, &NewFileDialog::OnTextEntered, this);

	tempoSizer->AddSpacer(40);
	tempoSizer->Add(text, 0, wxALIGN_CENTER_HORIZONTAL);
	tempoSizer->AddSpacer(40);
	tempoSizer->Add(textField, 0, wxALIGN_CENTER_HORIZONTAL);
	this->SetSizer(tempoSizer);
}

void NewFileDialog::PromptInstrument()
{
	BuildInstrumentList();
	text = new wxStaticText(this, wxID_ANY, "Select an instrument:");
	instrumentList = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 129, instruments);
	nextButton = new wxButton(this, wxID_ANY, "Enter");
	nextButton->Bind(wxEVT_BUTTON, &NewFileDialog::OnButtonPressInst, this);

	instrumentSizer->AddSpacer(20);
	instrumentSizer->Add(text, 0, wxALIGN_CENTER_HORIZONTAL);
	instrumentSizer->AddSpacer(20);
	instrumentSizer->Add(instrumentList, 0, wxALIGN_CENTER_HORIZONTAL);
	instrumentSizer->AddSpacer(20);
	instrumentSizer->Add(nextButton, 0, wxALIGN_CENTER_HORIZONTAL);


	this->SetSizer(instrumentSizer);
}

void NewFileDialog::PromptTimeSig()
{
	text = new wxStaticText(this, wxID_ANY, "Select a time signature:");
	BuildTimeSigLists();
	timeSigNum = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 19, sigNumList);
	timeSigDen = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, sigDenList);
	nextButton = new wxButton(this, wxID_ANY, "Enter");
	nextButton->Bind(wxEVT_BUTTON, &NewFileDialog::OnButtonPressSig, this);

	timeSigSizer->AddSpacer(20);
	timeSigSizer->Add(text, 0, wxALIGN_CENTER_HORIZONTAL);
	timeSigSizer->AddSpacer(20);
	timeSigSizer->Add(timeSigNum, 0, wxALIGN_CENTER_HORIZONTAL);
	timeSigSizer->AddSpacer(20);
	timeSigSizer->Add(timeSigDen, 0, wxALIGN_CENTER_HORIZONTAL);
	timeSigSizer->AddSpacer(20);
	timeSigSizer->Add(nextButton, 0, wxALIGN_CENTER_HORIZONTAL);

	this->SetSizer(timeSigSizer);

}

void NewFileDialog::OnTextEntered(wxCommandEvent& event)
{
	if (event.GetString() != "")
	{
		wxString tempoString = event.GetString();
		
		tempo = wxAtoi(tempoString);
		if (tempo > 0)
		{
			earlyExit = false;
			this->Close();
		}
		
	}
}

void NewFileDialog::OnButtonPressInst(wxCommandEvent& event)
{
	if(instrumentList->GetSelection() != wxNOT_FOUND)
	{ 
		instrumentCode = instrumentList->GetSelection() + 1;
		earlyExit = false;
		this->Close();
	}
}

void NewFileDialog::OnButtonPressSig(wxCommandEvent& event)
{
	if (timeSigDen->GetSelection() != wxNOT_FOUND && timeSigNum->GetSelection() != wxNOT_FOUND)
	{
		timeSig.y = timeSigNum->GetSelection() + 2;
		
		switch (timeSigDen->GetSelection())
		{
		case 0:
			timeSig.x = 4;
			break;
		case 1:
			timeSig.x = 8;
			break;
		case 2:
			timeSig.x = 16;
			break;
		}
		earlyExit = false;
		this->Close();
	}
}

void NewFileDialog::OnQuitDecision(wxCommandEvent& event)
{
	if (event.GetId() == quitID)
		doQuit = true;
	else
		doQuit = false;

	this->Close();
}

int NewFileDialog::GetTempo()
{
	return tempo;
}

int NewFileDialog::GetInstrument()
{
	return instrumentCode;
}

wxPoint NewFileDialog::GetTimeSignature()
{
	
	return timeSig;
}

bool NewFileDialog::GetEarlyExit()
{
	return earlyExit;
}

bool NewFileDialog::GetQuit()
{
	return doQuit;
}

void NewFileDialog::BuildTimeSigLists()
{
	for (int i = 2; i <= 21; i++)
	{
		sigNumList[i-2] = wxString::Format(wxT("%i"), i);
	}
	sigDenList[0] = "4";
	sigDenList[1] = "8";
	sigDenList[2] = "16";
}

void NewFileDialog::BuildInstrumentList()
{
	instruments[0] = "Piano 1";
	instruments[1] = "Piano 2";
	instruments[2] = "Piano 3";
	instruments[3] = "Honky Tonk Piano";
	instruments[4] = "Electric Piano 1";
	instruments[5] = "Electric Piano 2";
	instruments[6] = "Harpsicord";
	instruments[7] = "Clavinet";
	instruments[8] = "Celesta";
	instruments[9] = "Glockenspiel";
	instruments[10] = "Music Box";
	instruments[11] = "Vibraphone";
	instruments[12] = "Marimba";
	instruments[13] = "Xylophone";
	instruments[14] = "Tubular Bells";
	instruments[15] = "Dulcimar/Santoor";
	instruments[16] = "Organ 1";
	instruments[17] = "Organ 2";
	instruments[18] = "Organ 3";
	instruments[19] = "Organ 4";
	instruments[20] = "Organ 5";
	instruments[21] = "Accordian";
	instruments[22] = "Harmonica";
	instruments[23] = "Bandoneon";
	instruments[24] = "Acoustic Guitar (nylon)";
	instruments[25] = "Acoustic Guitar (steel)";
	instruments[26] = "Electric Guitar (jazz)";
	instruments[27] = "Electric Guitar (clean)";
	instruments[28] = "Electric Guitar (muted)";
	instruments[29] = "Electric Guitar (overdriven)";
	instruments[30] = "Electric Guitar (distortion)";
	instruments[31] = "Guitar Harmonics";
	instruments[32] = "Acoustic Bass";
	instruments[33] = "Electric Bass (finger)";
	instruments[34] = "Electric Bass (picked";
	instruments[35] = "Fretless Bass";
	instruments[36] = "Slap Bass 1";
	instruments[37] = "Slap Bass 2";
	instruments[38] = "Synth Bass 1";
	instruments[39] = "Synth Bass 2";
	instruments[40] = "Violin";
	instruments[41] = "Viola";
	instruments[42] = "Chello";
	instruments[43] = "Contrabass";
	instruments[44] = "Tremelo Strings";
	instruments[45] = "Pizzicato Strings";
	instruments[46] = "Orchestral Harp";
	instruments[47] = "Timpani";
	instruments[48] = "String Ensemble 1";
	instruments[49] = "String Ensemble 2";
	instruments[50] = "Synth Strings 1";
	instruments[51] = "Synth Strings 2";
	instruments[52] = "Voice 1";
	instruments[53] = "Voice 2";
	instruments[54] = "Synth Voice";
	instruments[55] = "Orchestra Hit";
	instruments[56] = "Trumpet";
	instruments[57] = "Trombone";
	instruments[58] = "Tuba";
	instruments[59] = "Muted Trumpet";
	instruments[60] = "French Horn";
	instruments[61] = "Brass Section";
	instruments[62] = "Synth Brass 1";
	instruments[63] = "Synth Brass 2";
	instruments[64] = "Soprano Sax";
	instruments[65] = "Alto Sax";
	instruments[66] = "Tenor Sax";
	instruments[67] = "Barritone Sax";
	instruments[68] = "Oboe";
	instruments[69] = "English Horn";
	instruments[70] = "Bassoon";
	instruments[71] = "Clarinet";
	instruments[72] = "Piccolo";
	instruments[73] = "Flute";
	instruments[74] = "Recorder";
	instruments[75] = "Pan Flute";
	instruments[76] = "Blown Bottle";
	instruments[77] = "Shakuhachi";
	instruments[78] = "Whistle";
	instruments[79] = "Ocarina";
	instruments[80] = "Synth Lead 1";
	instruments[81] = "Synth Lead 2";
	instruments[82] = "Synth Lead 3";
	instruments[83] = "Synth Lead 4";
	instruments[84] = "Synth Lead 5";
	instruments[85] = "Synth Lead 6";
	instruments[86] = "Synth Lead 7";
	instruments[87] = "Synth Lead 8";
	instruments[88] = "Synth Pad 1";
	instruments[89] = "Synth Pad 2";
	instruments[90] = "Synth Pad 3";
	instruments[91] = "Synth Pad 4";
	instruments[92] = "Synth Pad 5";
	instruments[93] = "Synth Pad 6";
	instruments[94] = "Synth Pad 7";
	instruments[95] = "Synth Pad 8";
	instruments[96] = "FX 1";
	instruments[97] = "FX 2";
	instruments[98] = "FX 3";
	instruments[99] = "FX 4";
	instruments[100] = "FX 5";
	instruments[101] = "FX 6";
	instruments[102] = "FX 7";
	instruments[103] = "FX 8";
	instruments[104] = "Sitar";
	instruments[105] = "Banjo";
	instruments[106] = "Shamisen";
	instruments[107] = "Koto";
	instruments[108] = "Kalimba";
	instruments[109] = "Bag Pipe";
	instruments[110] = "Fiddle";
	instruments[111] = "Shanai";
	instruments[112] = "Tinkle Bell";
	instruments[113] = "Cowbell";
	instruments[114] = "Steel Drums";
	instruments[115] = "Woodblock";
	instruments[116] = "Taiko Drum";
	instruments[117] = "Melodic Drum";
	instruments[118] = "Synth Tom";
	instruments[119] = "Reverse Cymbal";
	instruments[120] = "Guitar Fret Noise";
	instruments[121] = "Breath noise";
	instruments[122] = "Seashore";
	instruments[123] = "Bird Tweet";
	instruments[124] = "Telephone Ring";
	instruments[125] = "Helicopter";
	instruments[126] = "Applause";
	instruments[127] = "Gunshot";
	instruments[128] = "Drum Kit";
}

