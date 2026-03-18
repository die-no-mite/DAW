#include "trackFrame.h"
#include <wx/graphics.h>
#include <wx/dcbuffer.h>

TrackFrame::TrackFrame(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxPanel(parent, id, pos, size)
{
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
}