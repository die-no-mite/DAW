#pragma once


#include "objectspace.h"
#include "transformations.h"
#include "rect.h"

struct GraphicMIDIEvent
{
	wxRect2DDouble note;
	wxColor color;
	wxString text;
	wxAffineMatrix2D transform;
	Rect rect;
	/*
	GraphicMIDIEvent(wxRect2DDouble tnote, wxColor tcolor, wxString ttext, wxAffineMatrix2D ttransform, Transformation transformation = {})
		: boundingbox{ rect.rect }, transformation{ transformation }
	{
		wxRect2DDouble note = tnote;
		wxColor color = tcolor;
		wxString text = ttext;
		wxAffineMatrix2D transform = ttransform;
	}

	const wxRect2DDouble boundingbox;
	Transformation transformation;
	*/
};