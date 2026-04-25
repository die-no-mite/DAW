#pragma once

#include <wx/wx.h>

class NewFileDialog : public wxDialog
{
public:
	NewFileDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name, int prompt);
	std::string GetFileName();
	
	int GetTempo();
	wxPoint GetTimeSignature();
	int GetInstrument();
	bool GetEarlyExit();
	bool GetQuit();

private:

	void OnTextEntered(wxCommandEvent& event);
	void OnButtonPressInst(wxCommandEvent& event);
	void OnButtonPressSig(wxCommandEvent& event);
	void OnQuitDecision(wxCommandEvent& event);

	void BuildInstrumentList();
	void BuildTimeSigLists();

	void PromptTempo();
	void PromptInstrument();
	void PromptTimeSig();
	void PromptSave();

	wxStaticText* text;
	wxButton* nextButton;
	wxTextCtrl* textField;
	wxChoice* instrumentList;
	wxString instruments[129];
	wxChoice* timeSigNum;
	wxChoice* timeSigDen;
	wxString sigNumList[19];
	wxString sigDenList[3];

	wxBoxSizer* tempoSizer;
	wxBoxSizer* instrumentSizer;
	wxBoxSizer* timeSigSizer;
	wxBoxSizer* quitSizer;
	int stage = 0;
	int windowSize;
	wxString fileName;
	int tempo = 0;
	int instrumentCode;
	wxPoint timeSig;
	bool earlyExit = true;
	int quitID;
	bool doQuit = true;
};