#pragma once

#include <wx/wx.h>

class midiFrame : public wxWindow
{
public:
	midiFrame(wxWindow *parent, wxWindowID id, const wxPoint $pos, const wxSize &size);
	virtual ~midiFrame(){}
private:
	midiFrame(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size);
	void OnPaint(wxPaintEvent& evt);
	
};