#pragma once

#include <wx/wx.h>

class HelpDialog : public wxDialog
{
public:
	HelpDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name);

private:

	wxStaticText* text;
	wxButton* nextButton;

	wxBoxSizer* helpSizer;
	void OnButtonPress(wxCommandEvent& event);
	int tutorialPage = 0;
	int windowSize;
};