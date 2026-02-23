#pragma once

#include <wx/wx.h>

struct GraphicMIDIEvent
{
	wxRect2DDouble note;
	wxColor color;
	wxString text;
	wxAffineMatrix2D transform;
};