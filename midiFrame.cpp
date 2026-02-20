#include "midiFrame.h"
#include <wx/graphics.h>
#include <wx/dcbuffer.h>

midiFrame::midiFrame(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size) : wxWindow(parent, id, pos, size)
{
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);

	this->Bind(wxEVT_PAINT, &midiFrame::OnPaint, this);
}

void midiFrame::OnPaint(wxPaintEvent& evt)
{
	wxAutoBufferedPaintDC dc(this);
	dc.Clear();

	wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

	if (gc)
	{
		wxPoint rectOrigin = this->FromDIP(wxPoint(189, 200));
		wxSize rectSize = this->FromDIP(wxSize(100, 80));

		gc->SetBrush(*wxRED_BRUSH);
		gc->DrawRectangle(rectOrigin.x, rectOrigin.y, rectSize.GetWidth(), rectSize.GetHeight());

		delete gc;
	}
}